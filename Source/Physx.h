#ifndef PHYSX_H
#define PHYSX_H
#include <Utils/Utils.h>//用于与游戏的 Physx 引擎数据结构交互的接口
#include <numeric>
#include <set>
#include <functional>
#include "Utils/MeshPatcher.h"
#include <Common/math.h>
#include <DMALibrary/Memory/Memory.h>
#include <Common/Data.h>

// 读取 .obj 文件
std::vector<TriangleMeshData> ReadObjFile(const std::string& filePath) {
    std::vector<TriangleMeshData> meshDatas;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return meshDatas;
    }

    std::vector<Vector3> vertices;
    std::vector<uint32_t> indices;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "v") {
            float x, y, z;
            iss >> x >> y >> z;
            vertices.emplace_back(x, y, z);
        } else if (type == "f") {
            uint32_t v1, v2, v3;
            char slash;
            iss >> v1 >> slash >> slash >> v2 >> slash >> slash >> v3;
            // .obj 文件的索引从 1 开始，转换为 0 开始
            indices.push_back(v1 - 1);
            indices.push_back(v2 - 1);
            indices.push_back(v3 - 1);
        }
    }

    TriangleMeshData meshData;
    meshData.Vertices = vertices;
    meshData.Indices = indices;
    meshData.Flags = 0;
    meshData.QueryFilterData = {};
    meshData.SimulationFilterData = {};
    meshData.UniqueKey1 = {};
    meshData.Type = PxGeometryType::eTRIANGLEMESH;
    meshData.Transform = {};

    meshDatas.push_back(meshData);
    return meshDatas;
}
namespace physx
{
	enum class PxConcreteType : uint16_t
	{
		eUNDEFINED,

		eHEIGHTFIELD,
		eCONVEX_MESH,
		eTRIANGLE_MESH_BVH33,
		eTRIANGLE_MESH_BVH34,
		eCLOTH_FABRIC,

		eRIGID_DYNAMIC,
		eRIGID_STATIC,
		eSHAPE,
		eMATERIAL,
		eCONSTRAINT,
		eCLOTH,
		ePARTICLE_SYSTEM,
		ePARTICLE_FLUID,
		eAGGREGATE,
		eARTICULATION,
		eARTICULATION_LINK,
		eARTICULATION_JOINT,
		ePRUNING_STRUCTURE,

		ePHYSX_CORE_COUNT,
		eFIRST_PHYSX_EXTENSION = 256,
		eFIRST_VEHICLE_EXTENSION = 512,
		eFIRST_USER_EXTENSION = 1024
	};

	enum class PxBaseFlag : uint16_t
	{
		eOWNS_MEMORY = (1 << 0),
		eIS_RELEASABLE = (1 << 1)
	};

	enum class PxRigidBodyFlag : uint8_t
	{

		eKINEMATIC = (1 << 0), //!< Enable kinematic mode for the body.

		eUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES = (1 << 1),

		eENABLE_CCD = (1 << 2), //!< Enable CCD for the body.

		eENABLE_CCD_FRICTION = (1 << 3),

		eENABLE_POSE_INTEGRATION_PREVIEW = (1 << 4),

		eENABLE_SPECULATIVE_CCD = (1 << 5),

		eENABLE_CCD_MAX_CONTACT_IMPULSE = (1 << 6)
	};

	struct PxMatrix3x3T
	{

		Vector3 column0{};
		Vector3 column1{};
		Vector3 column2{};

		PxMatrix3x3T() = default;

		__forceinline PxMatrix3x3T(const Vector4& q)
		{
			// 提取四元数的分量
			const float x = q.x;
			const float y = q.y;
			const float z = q.z;
			const float w = q.w;

			// 预先计算 2 倍的分量值，避免重复计算
			const float twoX = 2.0f * x;
			const float twoY = 2.0f * y;
			const float twoZ = 2.0f * z;

			// 计算平方和乘积，避免重复计算
			const float xx = x * twoX;
			const float yy = y * twoY;
			const float zz = z * twoZ;
			const float xy = x * twoY;
			const float xz = x * twoZ;
			const float xw = w * twoX;
			const float yz = y * twoZ;
			const float yw = w * twoY;
			const float zw = w * twoZ;

			// 直接初始化列向量
			column0 = { 1.0f - yy - zz, xy + zw, xz - yw };
			column1 = { xy - zw, 1.0f - xx - zz, yz + xw };
			column2 = { xz + yw, yz - xw, 1.0f - xx - yy };
		}

		// 构造函数，使用列向量初始化矩阵
		PxMatrix3x3T(const Vector3& col0, const Vector3& col1, const Vector3& col2)
			: column0(col0), column1(col1), column2(col2)
		{
		}

		// 获取矩阵的转置
		__forceinline const PxMatrix3x3T getTranspose() const
		{
			return PxMatrix3x3T(
				{ column0.x, column1.x, column2.x },
				{ column0.y, column1.y, column2.y },
				{ column0.z, column1.z, column2.z }
			);
		}
		// 向量变换
		__forceinline const Vector3 transform(const Vector3& other) const
		{
			return {
				column0.x * other.x + column1.x * other.y + column2.x * other.z,
				column0.y * other.x + column1.y * other.y + column2.y * other.z,
				column0.z * other.x + column1.z * other.y + column2.z * other.z
			};
		}

		// 矩阵乘法
		__forceinline const PxMatrix3x3T operator*(const PxMatrix3x3T& other) const
		{
			// Rows from this <dot> columns from other
			return PxMatrix3x3T(
				transform(other.column0),
				transform(other.column1),
				transform(other.column2)
			);
		}

		inline const Vector3 operator*(const Vector3& vec) const
		{
			return transform(vec);
		}
	};

	struct PxsRigidCoreT
	{

		alignas(16) PxTransformT mBodyToWorld {};
		PxRigidBodyFlag Flags{};
		uint8_t m_idt_body_to_actor{};
		uint16_t m_solver_iteration_counts{};
	};

	struct BodyCoreT
	{
		char mPad[0x10]{};
		alignas(16) PxsRigidCoreT mCore {};
		alignas(16) PxTransformT mBodyToActor;
	};

	// rigid base class
	struct BodyT
	{
		uintptr_t mScene{};
		uintptr_t mControlState{};
		uintptr_t mStreamPtr{};
		BodyCoreT mRigid{};
	};

	// this is actually PxBase
	struct PxActorT
	{

		char mPad[0x8]{};
		PxConcreteType mType{};
		PxBaseFlag mBaseFlags{};
		uintptr_t m_user_data{};
	};

	struct PxGeometryT
	{
		PxGeometryType mType{};
	};

	struct PxBoxGeometry : PxGeometryT
	{

		// Half of the width, height, and depth of the box.
		Vector3 mHalfExtents{};

		bool valid()
		{

			if (mType != PxGeometryType::eBOX)
				return false;
			// if (!halfExtents.isFinite())
			//	return false;
			if (mHalfExtents.x <= 0.0f || mHalfExtents.y <= 0.0f || mHalfExtents.z <= 0.0f)
				return false;

			return true;
		}
	};

	struct PxSphereGeometryT : PxGeometryT
	{

		float mRadius{};

		bool valid()
		{

			if (mType != PxGeometryType::eSPHERE)
				return false;
			// if (!halfExtents.isFinite())
			//	return false;
			if (mRadius <= 0.f)
				return false;

			return true;
		}
	};

	struct PxCapsuleGeometryT : PxGeometryT
	{

		float mRadius{};
		float mHalfHeight{};

		bool valid()
		{

			if (mType != PxGeometryType::eCAPSULE)
				return false;
			// if (!halfExtents.isFinite())
			//	return false;
			if (mRadius <= 0.f || mHalfHeight <= 0.f)
				return false;

			return true;
		}
	};

	struct PxMeshScale
	{
	public:
		Vector3 transform(const Vector3& v) const
		{
			// 先旋转向量
			Vector3 rotated = rotation.rotate(v);
			// 对旋转后的向量进行缩放
			Vector3 scaled = scale.multiply(rotated);
			// 再进行逆旋转
			return rotation.rotateInv(scaled);
		}

		Vector3 scale;	  //!< A nonuniform scaling
		Vector4 rotation; //!< The orientation of the scaling axes
	};

	struct CenterExtentsT
	{

		Vector3 mCenter{};
		Vector3 mExtents{};
	};

	struct PxPlaneT
	{

		Vector3 n{};
		float d{};
	};

	struct HullPolygonDataT
	{

		PxPlaneT mPlane; //!< Plane equation for this polygon	//Could drop 4th elem as it can be computed from any vertex as: d = - p.dot(n);
		uint16_t mVRef8;   //!< Offset of vertex references in hull vertex data (CS: can we assume indices are tightly packed and offsets are ascending?? DrawObjects makes and uses this assumption)
		uint8_t mNbVerts;  //!< Number of vertices/edges in the polygon
		uint8_t mMinIndex; //!< Index of the polygon vertex that has minimal projection along this plane's normal.
	};

	template <typename storageType, storageType bitMask>
	class PxBitAndDataT
	{
	public:
		//__forceinline PxBitAndDataT(const PxEMPTY)
		//{
		//}
		// 默认构造函数，将 mData 初始化为 0
		__forceinline PxBitAndDataT() : mData(0)
		{
		}

		// 带参数的构造函数，根据 bit 的值设置 mData
		__forceinline PxBitAndDataT(storageType data, bool bit = false)
		{
			mData = bit ? storageType(data | bitMask) : data;
		}

		// 类型转换运算符，返回去除布尔标志位后的数据
		__forceinline operator storageType() const
		{
			return storageType(mData & ~bitMask);
		}

		// 设置布尔标志位为 1
		__forceinline void setBit()
		{
			mData |= bitMask;
		}

		// 清除布尔标志位为 0
		__forceinline void clearBit()
		{
			mData &= ~bitMask;
		}

		// 检查布尔标志位是否为 1
		__forceinline bool isBitSet() const
		{
			return (mData & bitMask) != 0;
		}

	protected:
		storageType mData;// 存储数据和布尔标志位的变量
	};
	typedef PxBitAndDataT<unsigned char, 0x80> PxBitAndByte;
	typedef PxBitAndDataT<unsigned short, 0x8000> PxBitAndWord;
	typedef PxBitAndDataT<unsigned int, 0x80000000> PxBitAndDword;

	// 凸包数据结构体
	struct ConvexHullDataT
	{
		// 凸包的 AABB 包围盒
		CenterExtentsT mAABB{};
		// 凸包的质心
		Vector3 m_center_of_mass{};
		// 边的数量，使用 PxBitAndWord 类型存储
		PxBitAndWord mNbEdges{};
		// 凸包顶点的数量
		uint8_t HullVerticesNb{};
		// 多边形的数量
		uint8_t PolygonsNb{};
		// 指向多边形数据的指针
		HullPolygonDataT* mPolygons{};

		// 获取凸包顶点的指针
		__forceinline const Vector3* getHullVertices() const
		{
			// 计算顶点数据的起始地址
			const char* vertexDataStart = reinterpret_cast<const char*>(mPolygons) + sizeof(HullPolygonDataT) * PolygonsNb;
			return reinterpret_cast<const Vector3*>(vertexDataStart);
		}

		// 获取顶点索引数据的指针
		__forceinline const uint8_t* getVertexData8() const
		{
			// 计算顶点数据的起始地址
			const char* vertexDataStart = reinterpret_cast<const char*>(mPolygons) + sizeof(HullPolygonDataT) * PolygonsNb;
			// 跳过顶点数据
			const char* current = vertexDataStart + sizeof(Vector3) * HullVerticesNb;
			// 跳过边的索引数据
			current += sizeof(uint8_t) * mNbEdges * 2;
			// 跳过顶点的额外数据
			current += sizeof(uint8_t) * HullVerticesNb * 3;
			// 如果边的数量标志位被设置，跳过额外的边索引数据
			if (mNbEdges.isBitSet())
			{
				current += sizeof(uint16_t) * mNbEdges * 2;
			}
			return reinterpret_cast<const uint8_t*>(current);
		}

		// 获取索引缓冲区的指针
		__forceinline const uint8_t* getIndexBuffer() const
		{
			// 提取边的数量（不包含标志位）
			unsigned short edgeCount = mNbEdges & 0x7FFF;
			// 计算索引缓冲区的起始地址
			const char* indexBufferStart = reinterpret_cast<const char*>(mPolygons) + 15 * HullVerticesNb + 2 * edgeCount;
			// 如果边的数量标志位被设置，跳过额外的边索引数据
			if ((mNbEdges & 0x8000u) != 0)
			{
				indexBufferStart += 4 * edgeCount;
			}
			return reinterpret_cast<const uint8_t*>(indexBufferStart);
		}

		// 获取每个边对应的相邻多边形数据的指针
		__forceinline const uint8_t* getFacesByEdges8() const
		{
			// 计算数据的起始地址
			const char* dataStart = reinterpret_cast<const char*>(mPolygons) + 20 * PolygonsNb + 12 * HullVerticesNb;
			return reinterpret_cast<const uint8_t*>(dataStart);
		}

		// 获取每个顶点对应的相邻多边形数据的指针
		__forceinline const uint8_t* getFacesByVertices8() const
		{
			// 计算数据的起始地址
			const char* dataStart = reinterpret_cast<const char*>(mPolygons) + 20 * PolygonsNb + 12 * HullVerticesNb;
			// 跳过边的额外数据
			dataStart += 1 * mNbEdges * 2;
			return reinterpret_cast<const uint8_t*>(dataStart);
		}
	};

	struct PxConvexMeshT
	{
	};

	struct ConvexMeshT : PxConvexMeshT
	{

		char mPad[0x8]{};
		PxConcreteType mType{};
		PxBaseFlag mBaseFlags{};
		uintptr_t mRefCountableVfptr{};
		int32_t mRefCount{};
		ConvexHullDataT HullData{};
		uint32_t mNb{};
	};

	struct PxConvexMeshGeometryT : PxGeometryT
	{

		PxMeshScale Scale{};
		ConvexMeshT* ConvexMesh{};
		float mMaxMargin{};
		uint8_t mMeshFlags{};
		char mFlagPad[0x3]{};
	};

	struct TriangleMeshT
	{

		char mPad[0x8]{};
		PxConcreteType mType{};
		PxBaseFlag mBaseFlags{};
		uintptr_t mRefCountableVfptr{};
		int64_t mRefCount{};
		uint32_t mNbVertices{};
		uint32_t mNbTriangles{};
		Vector3* Vertices{};
		void* mTriangles{};
		CenterExtentsT mAABB{};
		uint8_t* mExtraTrigData{};
		float mGeomEpsilon{};
		uint8_t Flags{};
	};

	struct PxTriangleMeshGeometryT : PxGeometryT
	{

		PxMeshScale Scale{};
		uint8_t mMeshFlags{};
		char mFlagPad[0x3]{};
		TriangleMeshT* mTriangleMesh{};
	};

	struct PxHeightFieldSampleT
	{

		int16_t mHeight{};
		PxBitAndByte mMaterialIndex0{};
		PxBitAndByte mMaterialIndex1{};
	};

	struct HeightFieldDataT
	{
		CenterExtentsT mAABB{};
		uint32_t Rows{};
		uint32_t Columns{};
		float mRowLimit{};
		float mColumnLimit{};
		float mNbColumns{};
		PxHeightFieldSampleT* mSamples{};
		float mThickness{};
		float mConvexEdgeThreshold{};
		uint16_t mFlags{};
		uint8_t mFormat{};
	};

	struct HeightFieldT
	{

		char mPad[0x8]{};
		PxConcreteType mType{};
		PxBaseFlag mBaseFlags{};
		uintptr_t mRefCountableVfptr{};
		int64_t mRefCount{};
		HeightFieldDataT mData{};
		uint32_t mSampleStride{};
		uint32_t mNbSamples{};
		float mMinHeight{};
		float mMaxHeight{};
		int32_t mModifyCount{};
		void* mMeshFactory{};
	};

	struct PxHeightFieldGeometryT : PxGeometryT
	{

		HeightFieldT* mHeightField{};
		float HeightScale{};
		float RowScale{};
		float ColumnScale{};
		int8_t Flags{};
		char mFlagPad[0x3]{};
	};

	struct GeometryUnionT
	{

		union
		{
			void* alignment; // PT: Makes sure the class is at least aligned to pointer size. See DE6803.
			uint8_t box[16];
			uint8_t sphere[8];
			uint8_t capsule[12];
			uint8_t plane[4];
			uint8_t convex[64];
			uint8_t mesh[80];
			uint8_t heightfield[56];
			uint8_t invalid[4];
		} mGeometry;

		__forceinline PxGeometryType getType() const { return reinterpret_cast<const PxGeometryT&>(mGeometry).mType; }
	};

	struct PxShapeCoreT
	{

		alignas(16) PxTransformT transform;
		float contactOffset;
		uint8_t mShapeFlags;			// !< API shape flags	// PT: TODO: use PxShapeFlags here. Needs to move flags to separate file.
		uint8_t mOwnsMaterialIdxMemory; // PT: for de-serialization to avoid deallocating material index list. Moved there from Sc::ShapeCore (since one byte was free).
		uint16_t materialIndex;
		GeometryUnionT geometry;
	};

	struct ShapeCoreT
	{

		FilterDataT QueryFilterData{};
		FilterDataT SimulationFilterData{};
		alignas(16) PxShapeCoreT mCore {};
		float mRestOffset{};

		__forceinline PxGeometryType getGeometryType() const { return mCore.geometry.getType(); }
		__forceinline const PxTransformT getShape2Actor() const { return mCore.transform; }
	};

	struct ShapeT
	{

		uintptr_t mScene{};
		uint32_t mControlState{};
		uintptr_t mStreamPtr{};

		ShapeCoreT ShapeCore{};

		__forceinline const PxGeometryT& GetGeometry() const { return reinterpret_cast<const PxGeometryT&>(ShapeCore.mCore.geometry.mGeometry); }
	};

	struct PxBounds3
	{
		Vector3 min, max;
	};

	struct PruningPoolT
	{
		uint32_t mNbObjects;
		uint32_t mMaxNbObjects;
		PxBounds3* mWorldBoxes;
		PrunerPayload* mObjects;
	};

	struct PrunerExtT
	{
		uintptr_t mPruner;
		char mPad[0x24]{};
		uint32_t mTimestamp;
	};

	struct NpSceneT
	{
		/// https://www.notion.so/PhysX-6eccab27717c47d09a07917c4640e386
		char mPad[0x2450]{};
		PrunerExtT exts[2];
	};

	struct ShapeDataT
	{
		ShapeT Shape{};
		BodyT Actor{};
		PxGeometryType mType{};

		uint8_t m_shape_flags{};

		PxMeshScale Scale{};

		PrunerPayload UniqueKey;

		// ConvexData
		ConvexMeshT ConvexMesh{};
		std::vector<HullPolygonDataT> polygons{};
		std::vector<uint8_t> ConvexIndices{};

		// TriangleData
		std::vector<uint32_t> Indices{};
		std::vector<uint16_t> SmallIndices{};
		TriangleMeshT TriangleMesh{};

		// ConvexData & TriangleData
		std::vector<Vector3> Vertices{};

		// HeightFieldData
		std::vector<PxHeightFieldSampleT> mSamples{};
		HeightFieldT mHeightField{};
	};

	template<typename T>
	void AddReadVecScatter(VMMDLL_SCATTER_HANDLE hScatter, uintptr_t address, size_t size, std::vector<T>* vec)
	{
		// 检查 vec 是否为 nullptr
		if (vec == nullptr) {
			return;
		}
		// 检查 size 是否为 0
		if (size == 0) {
			return;
		}
		// 若 vec 容量不足，预先分配足够的内存
		if (vec->capacity() < size) {
			vec->reserve(size);
		}
		// 调整 vec 的大小以匹配要读取的数据量
		vec->resize(size);
		// 调用 mem.AddScatterReadRequest 函数添加读取请求
		mem.AddScatterReadRequest(hScatter, address, vec->data(), sizeof(T) * size);


	}

	template<typename T>
	std::vector<T> ReadVec(uintptr_t address, size_t size, size_t custom_type_size = 0)
	{
		std::vector<T> temp{};

		// 检查数据大小，如果为 0 则直接返回空向量
		if (size == 0) {
			//std::cout << "Info: Size is 0, no data to read." << std::endl;
			return temp;
		}

		// 确定要读取的字节数
		size_t readSize = custom_type_size ? custom_type_size : sizeof(T) * size;

		// 调整向量大小以匹配要读取的数据量
		temp.resize(readSize / sizeof(T));

		// 尝试从指定地址读取数据
		if (mem.Read(address, temp.data(), readSize)) {
			//std::cout << "Info: Data read successfully." << std::endl;
			return temp;
		}
		else {
			//std::cerr << "Error: Failed to read data from address " << address << std::endl;
			// 清空向量以表示读取失败
			temp.clear();
			return temp;
		}
	}



	template <class _Pr>
	__forceinline std::vector<TriangleMeshData> GetMeshData(
		std::vector<PrunerPayload>& objects,
		_Pr filter,
		bool isDynamic = false,
		bool autoTransform = true
	) {
		std::vector<ShapeDataT> ShapeDatas(objects.size());
		std::vector<TriangleMeshData> TriangleMeshDatas;
		auto hScatter = mem.CreateScatterHandle();

		// 预分配内存
		ShapeDatas.reserve(objects.size());

		// Using index iteration instead of reference
		for (size_t i = 0; i < objects.size(); i++)
		{
			auto& obj = objects[i];
			ShapeDatas[i].UniqueKey = obj;
			mem.AddScatterReadRequest(hScatter, obj.Shape, &ShapeDatas[i].Shape, sizeof(ShapeT));
			mem.AddScatterReadRequest(hScatter, obj.Actor, &ShapeDatas[i].Actor, sizeof(BodyT));
		}
		mem.ExecuteReadScatter(hScatter);
		auto WorldLocation = GameData.Radar.WorldOriginLocation;

#ifdef _PHYSX_DEBUG
		//Utils::Log(1, "Number of objects: %d", objects.size());
#endif

	// 预计算标志和类型
		for (auto& ShapeData : ShapeDatas)
		{
			ShapeData.m_shape_flags = ShapeData.Shape.ShapeCore.mCore.mShapeFlags;
			ShapeData.mType = ShapeData.Shape.ShapeCore.getGeometryType();
		}

		// Filter
		ShapeDatas.erase(
			std::remove_if(ShapeDatas.begin(), ShapeDatas.end(), filter),
			ShapeDatas.end()
		);

		// 预分配内存
		for (auto& ShapeData : ShapeDatas)
		{
			if (ShapeData.mType == PxGeometryType::eHEIGHTFIELD)
			{
				PxHeightFieldGeometryT field_geometry = (PxHeightFieldGeometryT&)ShapeData.Shape.GetGeometry();
				mem.AddScatterReadRequest(hScatter, (uintptr_t)field_geometry.mHeightField, &ShapeData.mHeightField, sizeof(HeightFieldT));
			}
			else if (ShapeData.mType == PxGeometryType::eCONVEXMESH)
			{
				PxConvexMeshGeometryT convex_geometry = (PxConvexMeshGeometryT&)ShapeData.Shape.GetGeometry();
				mem.AddScatterReadRequest(hScatter, (uintptr_t)convex_geometry.ConvexMesh, &ShapeData.ConvexMesh, sizeof(ConvexMeshT));
			}
		}

		mem.ExecuteReadScatter(hScatter);

#ifdef _PHYSX_DEBUG
		//Utils::Log(1, "Number of ShapeDatas after filtering: %d", ShapeDatas.size());
#endif

	// 并行处理凸多边形数据
#pragma omp parallel for
		for (int i = 0; i < static_cast<int>(ShapeDatas.size()); i++)
		{
			auto& shapeData = ShapeDatas[i];
			if (shapeData.mType == PxGeometryType::eCONVEXMESH)
			{
				auto nbPolygons = shapeData.ConvexMesh.HullData.PolygonsNb;
				auto mPolygons = (uintptr_t)shapeData.ConvexMesh.HullData.mPolygons;
				auto size = sizeof(HullPolygonDataT);
				shapeData.polygons.resize(nbPolygons);

				for (uint32_t j = 0; j < nbPolygons; j++) {
					mem.AddScatterReadRequest<HullPolygonDataT>(hScatter, mPolygons + (j * size), &shapeData.polygons[j]);
				}
			}
		}

		mem.ExecuteReadScatter(hScatter);

		// 并行处理索引和顶点数据
#pragma omp parallel for
		for (int i = 0; i < static_cast<int>(ShapeDatas.size()); i++)
		{
			auto& ShapeData = ShapeDatas[i];
			if (ShapeData.mType == PxGeometryType::eCONVEXMESH)
			{
				uint32_t indices_number = std::accumulate(ShapeData.polygons.begin(), ShapeData.polygons.end(), 0u,
					[](uint32_t sum, const HullPolygonDataT& polygon) {
						return sum + polygon.mNbVerts;
					});
				ShapeData.ConvexIndices.reserve(indices_number);
				AddReadVecScatter<uint8_t>(hScatter, (uintptr_t)ShapeData.ConvexMesh.HullData.getVertexData8(), indices_number, &ShapeData.ConvexIndices);
			}
			else if (ShapeData.mType == PxGeometryType::eTRIANGLEMESH)
			{
				const PxTriangleMeshGeometryT& triangle_geometry = (PxTriangleMeshGeometryT&)ShapeData.Shape.GetGeometry();
				ShapeData.Scale = triangle_geometry.Scale;
				mem.AddScatterReadRequest<TriangleMeshT>(hScatter, (uintptr_t)triangle_geometry.mTriangleMesh, &ShapeData.TriangleMesh);
			}
		}
		mem.ExecuteReadScatter(hScatter);

		// 并行处理顶点和索引数据
#pragma omp parallel for
		for (int i = 0; i < static_cast<int>(ShapeDatas.size()); i++)
		{
			auto& ShapeData = ShapeDatas[i];
			if (ShapeData.mType == PxGeometryType::eTRIANGLEMESH)
			{
				bool has16BitIndices = (ShapeData.TriangleMesh.Flags & 2U) ? true : false;

				AddReadVecScatter(hScatter, (uintptr_t)ShapeData.TriangleMesh.Vertices, ShapeData.TriangleMesh.mNbVertices, &ShapeData.Vertices);

				if (has16BitIndices)
				{
					AddReadVecScatter(hScatter, (uintptr_t)ShapeData.TriangleMesh.mTriangles, ShapeData.TriangleMesh.mNbTriangles * 3, &ShapeData.SmallIndices);
				}
				else
				{
					AddReadVecScatter(hScatter, (uintptr_t)ShapeData.TriangleMesh.mTriangles, ShapeData.TriangleMesh.mNbTriangles * 3, &ShapeData.Indices);
				}
			}
			else if (ShapeData.mType == PxGeometryType::eHEIGHTFIELD)
			{
				const uint32_t nb = ShapeData.mHeightField.mNbSamples;
				AddReadVecScatter(hScatter, (uintptr_t)ShapeData.mHeightField.mData.mSamples, nb, &ShapeData.mSamples);
			}
			else if (ShapeData.mType == PxGeometryType::eCONVEXMESH)
			{
				AddReadVecScatter(hScatter, (uintptr_t)ShapeData.ConvexMesh.HullData.getHullVertices(), (size_t)ShapeData.ConvexMesh.HullData.HullVerticesNb, &ShapeData.Vertices);
			}
		}

		mem.ExecuteReadScatter(hScatter);
		mem.CloseScatterHandle(hScatter);

		// 并行处理小索引转换
#pragma omp parallel for
		for (int i = 0; i < static_cast<int>(ShapeDatas.size()); i++)
		{
			auto& ShapeData = ShapeDatas[i];
			if (ShapeData.mType == PxGeometryType::eTRIANGLEMESH && !ShapeData.SmallIndices.empty())
			{
				const size_t size = ShapeData.SmallIndices.size();
				ShapeData.Indices.resize(size);

				std::transform(
					ShapeData.SmallIndices.begin(),
					ShapeData.SmallIndices.end(),
					ShapeData.Indices.begin(),
					[](uint16_t val) { return static_cast<uint32_t>(val); }
				);

				ShapeData.SmallIndices.clear();
				ShapeData.SmallIndices.shrink_to_fit();
			}
		}

		// 并行处理网格数据生成
#pragma omp parallel for
		for (int i = 0; i < static_cast<int>(ShapeDatas.size()); i++)
		{
			const auto& ShapeDataItem = ShapeDatas[i];

			PxTransformT GlobalPose{};
			if (isDynamic) {
				GlobalPose = ShapeDataItem.Actor.mRigid.mCore.mBodyToWorld * ShapeDataItem.Actor.mRigid.mBodyToActor.getInverse();
			}
			else {
				GlobalPose = ShapeDataItem.Actor.mRigid.mCore.mBodyToWorld;
			}
			auto LocalPose = ShapeDataItem.Shape.ShapeCore.mCore.transform;
			auto CombinePose = GlobalPose * LocalPose;
			CombinePose.mPosition = CombinePose.mPosition + WorldLocation;
			auto UniqueKey = ShapeDataItem.UniqueKey;

			if (ShapeDataItem.mType == PxGeometryType::eBOX)
			{
				const PxBoxGeometry& boxGeometry = (PxBoxGeometry&)ShapeDataItem.Shape.GetGeometry();

				auto halfExtents = boxGeometry.mHalfExtents;

				Vector3 vertices[8];

				vertices[0] = Vector3(-halfExtents.x, -halfExtents.y, -halfExtents.z);
				vertices[1] = Vector3(halfExtents.x, -halfExtents.y, -halfExtents.z);
				vertices[2] = Vector3(halfExtents.x, halfExtents.y, -halfExtents.z);
				vertices[3] = Vector3(-halfExtents.x, halfExtents.y, -halfExtents.z);
				vertices[4] = Vector3(-halfExtents.x, -halfExtents.y, halfExtents.z);
				vertices[5] = Vector3(halfExtents.x, -halfExtents.y, halfExtents.z);
				vertices[6] = Vector3(halfExtents.x, halfExtents.y, halfExtents.z);
				vertices[7] = Vector3(-halfExtents.x, halfExtents.y, halfExtents.z);

				if (autoTransform) {
					for (int j = 0; j < 8; j++)
					{
						vertices[j] = CombinePose.transform(vertices[j]);
					}
				}

				// Box indices for triangles
				uint32_t indices[] = {
					// Front face
					0, 1, 2,
					0, 2, 3,
					// Back face
					4, 6, 5,
					4, 7, 6,
					// Top face
					0, 4, 5,
					0, 5, 1,
					// Bottom face
					2, 6, 7,
					2, 7, 3,
					// Right face
					0, 3, 7,
					0, 7, 4,
					// Left face
					1, 5, 6,
					1, 6, 2 };

				TriangleMeshData mesh_data{};
				mesh_data.Vertices.assign(vertices, vertices + 8);
				mesh_data.Indices.assign(indices, indices + 36);
				mesh_data.Flags = ShapeDataItem.m_shape_flags;
				mesh_data.QueryFilterData = ShapeDataItem.Shape.ShapeCore.QueryFilterData;
				mesh_data.SimulationFilterData = ShapeDataItem.Shape.ShapeCore.SimulationFilterData;
				mesh_data.UniqueKey1 = UniqueKey;
				mesh_data.Type = PxGeometryType::eBOX;
				mesh_data.Transform = CombinePose;

#pragma omp critical
				{
					TriangleMeshDatas.push_back(mesh_data);
				}
			}
			else if (ShapeDataItem.mType == PxGeometryType::eTRIANGLEMESH)
			{
				TriangleMeshData MeshData{};
				PxMeshScale ScaleVal = ShapeDataItem.Scale;

				const size_t vertexCount = ShapeDataItem.Vertices.size();
				MeshData.Vertices.resize(vertexCount);

				for (size_t j = 0; j < vertexCount; j++)
				{
					Vector3& vertex = MeshData.Vertices[j];
					vertex = ScaleVal.transform(ShapeDataItem.Vertices[j]);
					if (autoTransform) {
						vertex = CombinePose.transform(vertex);
					}
				}

				MeshData.Indices = ShapeDataItem.Indices;
				MeshData.Flags = ShapeDataItem.m_shape_flags;
				MeshData.QueryFilterData = ShapeDataItem.Shape.ShapeCore.QueryFilterData;
				MeshData.SimulationFilterData = ShapeDataItem.Shape.ShapeCore.SimulationFilterData;
				MeshData.UniqueKey1 = UniqueKey;
				MeshData.Type = PxGeometryType::eTRIANGLEMESH;
				MeshData.Transform = CombinePose;

#pragma omp critical
				{
					TriangleMeshDatas.push_back(MeshData);
				}
			}
			else if (ShapeDataItem.mType == PxGeometryType::eHEIGHTFIELD)
			{
				PxHeightFieldGeometryT field_geometry = (PxHeightFieldGeometryT&)ShapeDataItem.Shape.GetGeometry();

				auto NumRows = ShapeDataItem.mHeightField.mData.Rows;
				auto NumColumns = ShapeDataItem.mHeightField.mData.Columns;
				auto ColumnScale = field_geometry.ColumnScale;
				auto RowScale = field_geometry.RowScale;
				auto HeightScale = field_geometry.HeightScale;

				TriangleMeshData MeshData{};

				if (NumRows == 0 || NumColumns == 0 || ShapeDataItem.mSamples.empty()) {
					// fix unsign int
					continue;
				}

				for (uint32_t row = 0; row < NumRows - 1; row++) {
					for (uint32_t col = 0; col < NumColumns - 1; col++) {
						uint32_t idx00 = row * NumColumns + col;
						uint32_t idx10 = idx00 + 1;
						uint32_t idx01 = idx00 + NumColumns;
						uint32_t idx11 = idx01 + 1;

						auto sample00 = ShapeDataItem.mSamples[idx00];
						auto sample10 = ShapeDataItem.mSamples[idx10];
						auto sample01 = ShapeDataItem.mSamples[idx01];
						auto sample11 = ShapeDataItem.mSamples[idx11];
						Vector3 v00, v10, v01, v11;
						v00 = { row * RowScale, sample00.mHeight * HeightScale, col * ColumnScale };
						v10 = { row * RowScale, sample10.mHeight * HeightScale, (col + 1) * ColumnScale };
						v01 = { (row + 1) * RowScale, sample01.mHeight * HeightScale, col * ColumnScale };
						v11 = { (row + 1) * RowScale, sample11.mHeight * HeightScale, (col + 1) * ColumnScale };

						if (autoTransform)
						{
							v00 = CombinePose.transform(v00);
							v10 = CombinePose.transform(v10);
							v01 = CombinePose.transform(v01);
							v11 = CombinePose.transform(v11);
						}

						MeshData.Vertices.push_back(v00);
						MeshData.Vertices.push_back(v10);
						MeshData.Vertices.push_back(v01);
						MeshData.Vertices.push_back(v11);

						size_t base_idx = (row * (NumColumns - 1) + col) * 4;

						MeshData.Indices.push_back(base_idx);
						MeshData.Indices.push_back(base_idx + 1);
						MeshData.Indices.push_back(base_idx + 2);

						MeshData.Indices.push_back(base_idx + 1);
						MeshData.Indices.push_back(base_idx + 3);
						MeshData.Indices.push_back(base_idx + 2);
					}
				}

				MeshData.Flags = ShapeDataItem.m_shape_flags;
				MeshData.QueryFilterData = ShapeDataItem.Shape.ShapeCore.QueryFilterData;
				MeshData.SimulationFilterData = ShapeDataItem.Shape.ShapeCore.SimulationFilterData;
				MeshData.UniqueKey1 = UniqueKey;
				MeshData.UniqueKey2 = (uint64_t)((PxHeightFieldGeometryT&)ShapeDataItem.Shape.GetGeometry()).mHeightField;
				MeshData.Type = PxGeometryType::eHEIGHTFIELD;
				MeshData.Transform = CombinePose;

#pragma omp critical
				{
					TriangleMeshDatas.push_back(MeshData);
				}
			}
			else if (ShapeDataItem.mType == PxGeometryType::eCONVEXMESH)
			{
				PxConvexMeshGeometryT convex_geometry = (PxConvexMeshGeometryT&)ShapeDataItem.Shape.GetGeometry();
				const PxMeshScale ScaleVal = ShapeDataItem.Scale;
				const uint8_t nbVertices = ShapeDataItem.ConvexMesh.HullData.HullVerticesNb;
				const uint8_t nbPolygons = ShapeDataItem.ConvexMesh.HullData.PolygonsNb;
				TriangleMeshData MeshData{};

				MeshData.Vertices = ShapeDataItem.Vertices;
				const size_t vertexCount = MeshData.Vertices.size();

				for (size_t j = 0; j < vertexCount; j++) {
					MeshData.Vertices[j] = ScaleVal.transform(MeshData.Vertices[j]);
					if (autoTransform) {
						MeshData.Vertices[j] = CombinePose.transform(ScaleVal.transform(MeshData.Vertices[j]));
					}
				}
				for (const auto& polygon : ShapeDataItem.polygons) {
					for (uint16_t j = 0; j < polygon.mNbVerts - 2; j++) {
						uint32_t index_idx0 = polygon.mVRef8 + 0;
						uint32_t index_idx1 = polygon.mVRef8 + j + 1;
						uint32_t index_idx2 = polygon.mVRef8 + j + 2;

						if (index_idx0 >= ShapeDataItem.ConvexIndices.size() || index_idx2 >= ShapeDataItem.ConvexIndices.size()) {
							continue;
						}
						uint32_t idx0 = ShapeDataItem.ConvexIndices[index_idx0];
						uint32_t idx1 = ShapeDataItem.ConvexIndices[index_idx1];
						uint32_t idx2 = ShapeDataItem.ConvexIndices[index_idx2];

						MeshData.Indices.push_back(idx0);
						MeshData.Indices.push_back(idx1);
						MeshData.Indices.push_back(idx2);
					}
				}

				MeshData.Flags = ShapeDataItem.m_shape_flags;
				MeshData.QueryFilterData = ShapeDataItem.Shape.ShapeCore.QueryFilterData;
				MeshData.SimulationFilterData = ShapeDataItem.Shape.ShapeCore.SimulationFilterData;
				MeshData.UniqueKey1 = UniqueKey;
				MeshData.Type = PxGeometryType::eCONVEXMESH;
				MeshData.Transform = CombinePose;

				if (MeshData.QueryFilterData.word3 > 100) {
#pragma omp critical
					{
						TriangleMeshDatas.push_back(MeshData);
					}
				}
			}
		}

		// 遍历所有三角形网格数据
		for (auto& meshData : TriangleMeshDatas) {
			// 调用MeshPatcher的PatchMesh方法修补网格数据
			MeshPatcher::GetInstance()->PatchMesh(meshData);
		}

		// 返回包含所有三角形网格数据的向量
		return TriangleMeshDatas;
	}
	__forceinline std::vector<TriangleMeshData> LoadDynamicRigidShape(
		std::set<PrunerPayload>& currentSceneObjects,
		std::unordered_map<PrunerPayload, PxTransformT, PrunerPayloadHash>& cache,
		std::unordered_map<PrunerPayload, uint64_t, PrunerPayloadHash>& ptrCache,
		std::set<PrunerPayload>& willRemoveObjects,
		Vector3 currentPosition,
		double radius
	) {
		auto px_scene_arr_ptr = mem.Read<uint64_t>(GameData.PhysxInstancePtr + 0x8);
		auto px_scene_ptr = mem.Read<uint64_t>(px_scene_arr_ptr);
		auto scene = mem.Read<NpSceneT>(px_scene_ptr);
		auto pruner = mem.Read<PruningPoolT>(scene.exts[1].mPruner + 0x1A0);
		std::vector<PrunerPayload> mObjects = ReadVec<PrunerPayload>((uintptr_t)pruner.mObjects, pruner.mNbObjects);

		std::set<uint64_t> queryActor{};
		for (const auto& obj : mObjects) {
			queryActor.insert(obj.Actor);
		}

		auto hScatter = mem.CreateScatterHandle();

		std::unordered_map<uint64_t, BodyT> actorPos{};
		std::unordered_map<uint64_t, ShapeT> shapePos{};
		actorPos.reserve(queryActor.size());
		shapePos.reserve(mObjects.size());

		for (const auto& actor : queryActor)
		{
			actorPos[actor] = {};
			mem.AddScatterReadRequest(hScatter, actor, &actorPos[actor], sizeof(BodyT));
		}
		for (const auto& obj : mObjects) {
			shapePos[obj.Shape] = {};
			mem.AddScatterReadRequest(hScatter, obj.Shape, &shapePos[obj.Shape], sizeof(ShapeT));
		}
		mem.ExecuteReadScatter(hScatter);
		mem.CloseScatterHandle(hScatter);

		std::set<PrunerPayload> removeObjects{};
		std::vector<PrunerPayload> addObjects{};
		for (const auto& obj : mObjects) {
			auto& body = actorPos[obj.Actor];
			auto& shape = shapePos[obj.Shape];
			PxTransformT GlobalPose = (body.mRigid.mCore.mBodyToWorld * body.mRigid.mBodyToActor.getInverse()) * shape.ShapeCore.mCore.transform;
			GlobalPose.mPosition = GlobalPose.mPosition + GameData.Radar.WorldOriginLocation;
			auto it = cache.find(obj);
			if (it != cache.end()) {
				if (!it->second.IsNearlyEqual(GlobalPose)) {
					// Transform change, remove it;
					currentSceneObjects.erase(obj);
					willRemoveObjects.insert(obj);
				}
			}
			cache[obj] = GlobalPose;
			auto ptrIt = ptrCache.find(obj);
			auto trianglePtr = (uint64_t)((PxTriangleMeshGeometryT&)shape.GetGeometry()).mTriangleMesh;
			if (ptrIt != ptrCache.end()) {
				if (ptrIt->second != trianglePtr) {
					// ptr change, remove it.
					currentSceneObjects.erase(obj);
					willRemoveObjects.insert(obj);
				}
			}
			ptrCache[obj] = trianglePtr;
		}

		// 删除缓存中过期的Shape
		std::set<PrunerPayload> mObjectsSet(mObjects.begin(), mObjects.end());
		for (auto it = cache.begin(); it != cache.end(); ) {
			if (mObjectsSet.find(it->first) == mObjectsSet.end()) {
				it = cache.erase(it);
			}
			else {
				++it;
			}
		}
		for (auto it = ptrCache.begin(); it != ptrCache.end(); ) {
			if (mObjectsSet.find(it->first) == mObjectsSet.end()) {
				it = ptrCache.erase(it);
			}
			else {
				++it;
			}
		}


		double radiusSquare = radius * radius;

		// 根据缓存过滤指定范围的Shape
		std::set<PrunerPayload> result;
		for (const auto& [obj, pos] : cache) {
			if ((pos.mPosition - currentPosition).Length2DSquare() < radiusSquare) {
				result.insert(obj);
			}
		}

		// 判断过滤后的Shape是否已经在场景中被加载
		std::copy_if(currentSceneObjects.begin(), currentSceneObjects.end(), std::inserter(removeObjects, removeObjects.begin()),
			[&result](const PrunerPayload& obj) { return result.find(obj) == result.end(); });

		std::copy_if(
			result.begin(), result.end(), std::back_inserter(addObjects),
			[&currentSceneObjects](const PrunerPayload& obj) {
				return currentSceneObjects.find(obj) == currentSceneObjects.end();
			}
		);
		//Utils::Log(0, "Dynamic -> Remove objects: %d, Add objects: %d", removeObjects.size(), addObjects.size());

		// 将多余的Shape从场景中移除，将少的Shape加入场景
		for (const auto& obj : removeObjects) {
			currentSceneObjects.erase(obj);
			willRemoveObjects.insert(obj);
		}

		for (const auto& obj : addObjects) {
			currentSceneObjects.insert(obj);
		}

		return GetMeshData(
			addObjects,
			[](const ShapeDataT& shape_data) {
				PxGeometryType type = shape_data.mType;
				return type != PxGeometryType::eTRIANGLEMESH;
			},
			true
		);
	}

	/// <summary>
	/// Refresh current Game Height Field and compare with history ptr
	/// Note: Don't filter HeightField ptr, because HeightField edit internal ptr not recreate HeightField
	/// </summary>
	/// <param name="lastTimestamp"></param>
	/// <param name="UniqueKeySet"></param>
	/// <param name="HeightFieldSet">Height field shape ptr</param>
	/// <param name="HeightFieldSamplePtrSet">Internal ptr for filtering in height field</param>
	/// <param name="RemoveHeightFieldKey">Should Remove Height Field</param>
	/// <returns></returns>
	__forceinline std::vector<TriangleMeshData> RefreshDynamicLoadHeightField(
		uint32_t& lastTimestamp,
		std::set<PrunerPayload>& UniqueKeySet,
		std::set<PrunerPayload>& HeightFieldSet,
		std::set<uint64_t>& HeightFieldSamplePtrSet,
		std::set<uint64_t>& RemoveHeightFieldKey
	) {
		auto px_scene_arr_ptr = mem.Read<uint64_t>(GameData.PhysxInstancePtr + 0x8);
		auto px_scene_ptr = mem.Read<uint64_t>(px_scene_arr_ptr);
		auto scene = mem.Read<NpSceneT>(px_scene_ptr);

		if (lastTimestamp - scene.exts->mTimestamp < 1000) {
			return std::vector<TriangleMeshData>{};
		}
		lastTimestamp = scene.exts->mTimestamp;

		auto pruner = mem.Read<PruningPoolT>(scene.exts[0].mPruner + 0x1A0);
		auto dynamicPruner = mem.Read<PruningPoolT>(scene.exts[1].mPruner + 0x1A0);
		//Utils::Log(3, "Dynamic object num: %d", dynamicPruner.mNbObjects);
		std::vector<PrunerPayload> mObjects = ReadVec<PrunerPayload>((uintptr_t)pruner.mObjects, pruner.mNbObjects);
		std::set<PrunerPayload> newUnionKeySet(mObjects.begin(), mObjects.end());
		std::vector<PrunerPayload> newObjects;

		std::copy_if(mObjects.begin(), mObjects.end(), std::back_inserter(newObjects),
			[&UniqueKeySet, &HeightFieldSet](const PrunerPayload& obj) {
				// if current ptr not contains or current ptr is height field, load it data!
				return UniqueKeySet.find(obj) == UniqueKeySet.end() || HeightFieldSet.find(obj) != HeightFieldSet.end();
			});

		if (newObjects.size() < 1500) {
			return std::vector<TriangleMeshData>{};
		}

		UniqueKeySet.insert(newObjects.begin(), newObjects.end());

		std::set<PrunerPayload> RemoveKey{};

		std::set_difference(UniqueKeySet.begin(), UniqueKeySet.end(),
			newUnionKeySet.begin(), newUnionKeySet.end(),
			std::inserter(RemoveKey, RemoveKey.begin()));


		for (const auto& key : RemoveKey) {
			UniqueKeySet.erase(key);
		}
#ifdef _PHYSX_DEBUG
		//Utils::Log(1, "Trigger rebuild, New objects: %d", newObjects.size());
#endif


		std::set<uint64_t> currentHeightFieldPtrSet{};

		auto result = GetMeshData(
			newObjects,
			[&HeightFieldSamplePtrSet, &currentHeightFieldPtrSet, &HeightFieldSet](const ShapeDataT& shape_data) {
				PxGeometryType type = shape_data.mType;
				if (type == PxGeometryType::eHEIGHTFIELD)
				{
					PxHeightFieldGeometryT field_geometry = (PxHeightFieldGeometryT&)shape_data.Shape.GetGeometry();
					HeightFieldSet.insert(shape_data.UniqueKey);
					// vaild height field, add current height field
					if (field_geometry.RowScale == 100.f) {
						auto ptr = (uintptr_t)field_geometry.mHeightField;
						currentHeightFieldPtrSet.insert(ptr);
						return !HeightFieldSamplePtrSet.insert(ptr).second;
					}
					// filter fuzzy height field
					return true;
				}
				return true;
			}
		);


		std::set_difference(HeightFieldSamplePtrSet.begin(), HeightFieldSamplePtrSet.end(),
			currentHeightFieldPtrSet.begin(), currentHeightFieldPtrSet.end(),
			std::inserter(RemoveHeightFieldKey, RemoveHeightFieldKey.begin()));

		for (const auto& key : RemoveHeightFieldKey) {
			HeightFieldSamplePtrSet.erase(key);
		}

		return result;
	}

	/// <summary>
	/// Load shape by range
	/// </summary>
	/// <param name="cache">Cache of shape position</param>
	/// <param name="currentSceneObjects">Current embree scene objects</param>
	/// <param name="willRemoveObjects">Will remove objects from embree scene</param>
	/// <param name="currentPosition">Current position</param>
	/// <param name="radius">Radius</param>
	/// <param name="limit">Limit</param>
	/// <param name="isDynamic">Is dynamic rigid</param>
	__forceinline std::vector<TriangleMeshData> LoadShapeByRange(
		uint32_t& lastTimestamp,
		std::unordered_map<PrunerPayload, PxTransformT, PrunerPayloadHash>& cache,
		std::set<PrunerPayload>& currentSceneObjects,
		std::set<PrunerPayload>& willRemoveObjects,
		std::unordered_map<PrunerPayload, uint64_t, PrunerPayloadHash>& alwaysCheckShape,
		Vector3 currentPosition,
		double radius,
		uint32_t limit = 200
	) {
		auto px_scene_arr_ptr = mem.Read<uint64_t>(GameData.PhysxInstancePtr + 0x8);
		auto px_scene_ptr = mem.Read<uint64_t>(px_scene_arr_ptr);
		auto scene = mem.Read<NpSceneT>(px_scene_ptr);

		if (lastTimestamp - scene.exts->mTimestamp < 1000) {
			return std::vector<TriangleMeshData>{};
		}
		lastTimestamp = scene.exts->mTimestamp;

		auto pruner = mem.Read<PruningPoolT>(scene.exts[0].mPruner + 0x1A0);
		std::vector<PrunerPayload> mObjects = ReadVec<PrunerPayload>((uintptr_t)pruner.mObjects, pruner.mNbObjects);

		// 查询当前场景中没有被缓存的Shape，并加入缓存，
		std::set<PrunerPayload> queryObjects{};
		std::set<PrunerPayload> mObjectsSet(mObjects.begin(), mObjects.end());
		for (const auto& obj : mObjectsSet) {
			if (cache.find(obj) == cache.end()) {
				queryObjects.insert(obj);
			}
		}

		std::set<uint64_t> queryActor{};
		for (const auto& obj : queryObjects) {
			queryActor.insert(obj.Actor);
		}

		auto hScatter = mem.CreateScatterHandle();

		std::unordered_map<uint64_t, BodyT> actorPos{};
		std::unordered_map<uint64_t, ShapeT> shapePos{};
		actorPos.reserve(queryActor.size());
		shapePos.reserve(queryObjects.size());

		for (const auto& actor : queryActor)
		{
			actorPos[actor] = {};
			mem.AddScatterReadRequest(hScatter, actor, &actorPos[actor], sizeof(BodyT));
		}
		for (const auto& obj : queryObjects) {
			shapePos[obj.Shape] = {};
			mem.AddScatterReadRequest(hScatter, obj.Shape, &shapePos[obj.Shape], sizeof(ShapeT));
		}
		mem.ExecuteReadScatter(hScatter);
		mem.CloseScatterHandle(hScatter);

		// 更新新查询到的Shape的位置
		for (const auto& obj : queryObjects) {
			auto& body = actorPos[obj.Actor];
			auto& shape = shapePos[obj.Shape];
			PxTransformT GlobalPose = body.mRigid.mCore.mBodyToWorld * shape.ShapeCore.mCore.transform;
			GlobalPose.mPosition = GlobalPose.mPosition + GameData.Radar.WorldOriginLocation;
			cache[obj] = GlobalPose;
		}

		std::set<PrunerPayload> removeObjects{};
		std::vector<PrunerPayload> addObjects{};

		// 删除缓存中过期的Shape
		for (auto it = cache.begin(); it != cache.end(); ) {
			if (mObjectsSet.find(it->first) == mObjectsSet.end()) {
				it = cache.erase(it);
			}
			else {
				++it;
			}
		}

		double radiusSquare = radius * radius;

		// 根据缓存过滤指定范围的Shape
		std::set<PrunerPayload> result;
		for (const auto& [obj, pos] : cache) {
			if ((pos.mPosition - currentPosition).Length2DSquare() < radiusSquare) {
				result.insert(obj);
			}
		}

		// 判断过滤后的Shape是否已经在场景中被加载，如果变动的Shape数量过少则放弃变动

		std::copy_if(currentSceneObjects.begin(), currentSceneObjects.end(), std::inserter(removeObjects, removeObjects.begin()),
			[&result](const PrunerPayload& obj) { return result.find(obj) == result.end(); });

		std::copy_if(
			result.begin(), result.end(), std::back_inserter(addObjects),
			[&currentSceneObjects, &alwaysCheckShape](const PrunerPayload& obj) {
				return currentSceneObjects.find(obj) == currentSceneObjects.end() || alwaysCheckShape.find(obj) != alwaysCheckShape.end();
			}
		);

		if (removeObjects.size() + addObjects.size() < limit) {
			//Utils::Log(1, "Static -> Remove objects: %d, Add objects: %d", removeObjects.size(), addObjects.size());
			return std::vector<TriangleMeshData>{};
		}
		//Utils::Log(0, "Static -> Remove objects: %d, Add objects: %d", removeObjects.size(), addObjects.size());

		// 将多余的Shape从场景中移除，将少的Shape加入场景
		for (const auto& obj : removeObjects) {
			currentSceneObjects.erase(obj);
			alwaysCheckShape.erase(obj);
			willRemoveObjects.insert(obj);
		}

		for (const auto& obj : addObjects) {
			currentSceneObjects.insert(obj);
		}

		auto res = GetMeshData(
			addObjects,
			[&alwaysCheckShape, &willRemoveObjects, &currentSceneObjects](const ShapeDataT& shape_data) {
				PxGeometryType type = shape_data.mType;
				uint8_t Flags = shape_data.m_shape_flags;
				auto queryFlag = shape_data.Shape.ShapeCore.QueryFilterData;
				if (queryFlag.word0 > 0uLL && queryFlag.word2 > 0uLL && type == PxGeometryType::eTRIANGLEMESH) {
					// break able wall
					auto findRes = alwaysCheckShape.find(shape_data.UniqueKey);
					auto trianglePtr = (uint64_t)((PxTriangleMeshGeometryT&)shape_data.Shape.GetGeometry()).mTriangleMesh;
					if (findRes == alwaysCheckShape.end()) {
						alwaysCheckShape.insert({ shape_data.UniqueKey, trianglePtr });
					}
					else {
						if (findRes->second == trianglePtr) {
							// not change, filter it.
							return true;
						}
						else {
							alwaysCheckShape[shape_data.UniqueKey] = trianglePtr;
							willRemoveObjects.insert(shape_data.UniqueKey);
							return false;
						}
					}
				}
				return type != PxGeometryType::eTRIANGLEMESH || ((Flags & 2) == 0 && (Flags & 1) == 0);
			}
		);


		return res;
	}
}
#endif
