//
// Copyright (c) 2021-2023 Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include <tutorials/common/TutorialBase.h>

class Tutorial : public TutorialBase
{
  public:
	void run() 
	{
		const int	sphereCount = 8 * 2;
		hiprtFloat4 spheres[sphereCount];
		for ( int i = 0; i < sphereCount / 2; i++ )
		{
			float r	   = 0.1f;
			float t	   = i / (float)( sphereCount / 2 ) * 2.f * 3.1415f;
			spheres[i] = { sin( t ) * 0.4f, cos( t ) * 0.4f, 0.f, r };
		}
		for ( int i = 0; i < sphereCount / 2; i++ )
		{
			float r					  = 0.1f;
			float t					  = i / (float)( sphereCount / 2 ) * 2.f * 3.1415f + 0.2f;
			spheres[i + sphereCount / 2] = { sin( t ) * 0.35f, cos( t ) * 0.35f, 0.4f, r };
		}

		hiprtContext ctxt;
		CHECK_HIPRT( hiprtCreateContext( HIPRT_API_VERSION, m_ctxtInput, &ctxt ) );

		hiprtAABBListPrimitive list;
		list.aabbCount	= sphereCount;
		list.aabbStride = 2 * sizeof( hiprtFloat4 );
		hiprtFloat4 aabbs[2 * sphereCount];
		for ( int i = 0; i < sphereCount ; i++)
		{
			const hiprtFloat4& c = spheres[i];
			aabbs[i * 2 + 0]	 = { c.x - c.w, c.y - c.w, c.z - c.w, 0.0f };
			aabbs[i * 2 + 1]	 = { c.x + c.w, c.y + c.w, c.z + c.w, 0.0f };
		}
		CHECK_ORO( oroMalloc( (oroDeviceptr*)&list.aabbs, 2 * sphereCount * sizeof( hiprtFloat4 ) ) );
		CHECK_ORO( oroMemcpyHtoD( (oroDeviceptr)list.aabbs, aabbs, 2 * sphereCount * sizeof( hiprtFloat4 ) ) );

		hiprtGeometryBuildInput geomInput;
		geomInput.type				 = hiprtPrimitiveTypeAABBList;
		geomInput.aabbList.primitive = &list;
		geomInput.geomType			 = 0;

		size_t			  geomTempSize;
		hiprtDevicePtr	  geomTemp;
		hiprtBuildOptions options;
		options.buildFlags = hiprtBuildFlagBitPreferFastBuild;
		CHECK_HIPRT( hiprtGetGeometryBuildTemporaryBufferSize( ctxt, &geomInput, &options, &geomTempSize ) );
		CHECK_ORO( oroMalloc( (oroDeviceptr*)&geomTemp, geomTempSize ) );

		hiprtGeometry geom;
		CHECK_HIPRT( hiprtCreateGeometry( ctxt, &geomInput, &options, &geom ) );
		CHECK_HIPRT( hiprtBuildGeometry( ctxt, hiprtBuildOperationBuild, &geomInput, &options, geomTemp, 0, geom ) );

		hiprtFuncNameSet funcNameSet;
		funcNameSet.intersectFuncName			   = "intersectSphere";
		std::vector<hiprtFuncNameSet> funcNameSets = { funcNameSet };

		hiprtFuncDataSet funcDataSet;
		CHECK_ORO( oroMalloc( (oroDeviceptr*)&funcDataSet.intersectFuncData, sphereCount * sizeof( hiprtFloat4 ) ) );
		CHECK_ORO(
			oroMemcpyHtoD( (oroDeviceptr)funcDataSet.intersectFuncData, spheres, sphereCount * sizeof( hiprtFloat4 ) ) );

		hiprtFuncTable funcTable;
		CHECK_HIPRT( hiprtCreateFuncTable( ctxt, 1, 1, &funcTable ) );
		CHECK_HIPRT( hiprtSetFuncTable( ctxt, funcTable, 0, 0, funcDataSet ) );

		oroFunction		 func;
		buildTraceKernelFromBitcode( ctxt, "../common/TutorialKernels.h", "CustomIntersectionKernel", func, nullptr, &funcNameSets, 1, 1 );

		u8* pixels;
		CHECK_ORO( oroMalloc( (oroDeviceptr*)&pixels, m_res.x * m_res.y * 4 ) );

		void* args[] = { &geom, &pixels, &funcTable, &m_res };
		launchKernel( func, m_res.x, m_res.y, args );
		writeImage( "03_custom_intersection.png", m_res.x, m_res.y, pixels );

		CHECK_ORO( oroFree( (oroDeviceptr)funcDataSet.intersectFuncData ) );
		CHECK_ORO( oroFree( (oroDeviceptr)list.aabbs ) );
		CHECK_ORO( oroFree( (oroDeviceptr)geomTemp ) );
		CHECK_ORO( oroFree( (oroDeviceptr)pixels ) );

		CHECK_HIPRT( hiprtDestroyFuncTable( ctxt, funcTable ) );
		CHECK_HIPRT( hiprtDestroyGeometry( ctxt, geom ) );
		CHECK_HIPRT( hiprtDestroyContext( ctxt ) );
	}
};

int main( int argc, char** argv )
{
	Tutorial tutorial;
	tutorial.init( 0 );
	tutorial.run();

	return 0;
}


