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
		hiprtContext ctxt;
		CHECK_HIPRT( hiprtCreateContext( HIPRT_API_VERSION, m_ctxtInput, &ctxt ) );

		hiprtGeometry			   geom0;
		hiprtTriangleMeshPrimitive mesh0;
		{
			mesh0.triangleCount	  = 1;
			mesh0.triangleStride  = sizeof( hiprtInt3 );
			int triangleIndices[] = { 0, 1, 2 };
			CHECK_ORO( oroMalloc( (oroDeviceptr*)&mesh0.triangleIndices, mesh0.triangleCount * sizeof( hiprtInt3 ) ) );
			CHECK_ORO( oroMemcpyHtoD(
				(oroDeviceptr)mesh0.triangleIndices, triangleIndices, mesh0.triangleCount * sizeof( hiprtInt3 ) ) );

			mesh0.vertexCount	   = 3;
			mesh0.vertexStride	   = sizeof( hiprtFloat3 );
			const float s		   = 0.15f;
			hiprtFloat3 vertices[] = {
				{ s * sin( 0.0f ), s * cos( 0.0f ), 0.0f },
				{ s * sin( Pi * 2.0f / 3.0f ), s * cos( Pi * 2.0f / 3.0f ), 0.0f },
				{ s * sin( Pi * 4.0f / 3.0f ), s * cos( Pi * 4.0f / 3.0f ), 0.0f } };
			CHECK_ORO( oroMalloc( (oroDeviceptr*)&mesh0.vertices, mesh0.vertexCount * sizeof( hiprtFloat3 ) ) );
			CHECK_ORO( oroMemcpyHtoD( (oroDeviceptr)mesh0.vertices, vertices, mesh0.vertexCount * sizeof( hiprtFloat3 ) ) );

			hiprtGeometryBuildInput geomInput;
			geomInput.type					 = hiprtPrimitiveTypeTriangleMesh;
			geomInput.triangleMesh.primitive = &mesh0;

			size_t			  geomTempSize;
			hiprtDevicePtr	  geomTemp;
			hiprtBuildOptions options;
			options.buildFlags = hiprtBuildFlagBitPreferFastBuild;
			CHECK_HIPRT( hiprtGetGeometryBuildTemporaryBufferSize( ctxt, &geomInput, &options, &geomTempSize ) );
			CHECK_ORO( oroMalloc( (oroDeviceptr*)&geomTemp, geomTempSize ) );

			CHECK_HIPRT( hiprtCreateGeometry( ctxt, &geomInput, &options, &geom0 ) );
			CHECK_HIPRT( hiprtBuildGeometry( ctxt, hiprtBuildOperationBuild, &geomInput, &options, geomTemp, 0, geom0 ) );
			CHECK_ORO( oroFree( (oroDeviceptr)geomTemp ) );
		}

		hiprtGeometry			   geom1;
		hiprtTriangleMeshPrimitive mesh1;
		{
			mesh1.triangleCount	  = 1;
			mesh1.triangleStride  = sizeof( hiprtInt3 );
			int triangleIndices[] = { 0, 1, 2 };
			CHECK_ORO( oroMalloc( (oroDeviceptr*)&mesh1.triangleIndices, mesh1.triangleCount * sizeof( hiprtInt3 ) ) );
			CHECK_ORO( oroMemcpyHtoD(
				(oroDeviceptr)mesh1.triangleIndices, triangleIndices, mesh1.triangleCount * sizeof( hiprtInt3 ) ) );

			mesh1.vertexCount	   = 3;
			mesh1.vertexStride	   = sizeof( hiprtFloat3 );
			const float s		   = 0.15f;
			hiprtFloat3 vertices[] = {
				{ s * sin( 0.0f ), s * cos( 0.0f ), 0.0f },
				{ s * sin( Pi * 2.0f / 3.0f ), s * cos( Pi * 2.0f / 3.0f ), 0.0f },
				{ s * sin( Pi * 4.0f / 3.0f ), s * cos( Pi * 4.0f / 3.0f ), 0.0f } };
			CHECK_ORO( oroMalloc( (oroDeviceptr*)&mesh1.vertices, mesh1.vertexCount * sizeof( hiprtFloat3 ) ) );
			CHECK_ORO( oroMemcpyHtoD( (oroDeviceptr)mesh1.vertices, vertices, mesh1.vertexCount * sizeof( hiprtFloat3 ) ) );

			hiprtGeometryBuildInput geomInput;
			geomInput.type					 = hiprtPrimitiveTypeTriangleMesh;
			geomInput.triangleMesh.primitive = &mesh1;

			size_t			  geomTempSize;
			hiprtDevicePtr	  geomTemp;
			hiprtBuildOptions options;
			options.buildFlags = hiprtBuildFlagBitPreferFastBuild;
			CHECK_HIPRT( hiprtGetGeometryBuildTemporaryBufferSize( ctxt, &geomInput, &options, &geomTempSize ) );
			CHECK_ORO( oroMalloc( (oroDeviceptr*)&geomTemp, geomTempSize ) );

			CHECK_HIPRT( hiprtCreateGeometry( ctxt, &geomInput, &options, &geom1 ) );
			CHECK_HIPRT( hiprtBuildGeometry( ctxt, hiprtBuildOperationBuild, &geomInput, &options, geomTemp, 0, geom1 ) );
			CHECK_ORO( oroFree( (oroDeviceptr)geomTemp ) );
		}

		hiprtScene			 scene;
		hiprtSceneBuildInput sceneInput;
		{
			sceneInput.instanceCount = 2;
			sceneInput.instanceMasks = nullptr;
			hiprtDevicePtr geoms[]	 = { geom0, geom1 };
			CHECK_ORO( oroMalloc(
				(oroDeviceptr*)&sceneInput.instanceGeometries, sceneInput.instanceCount * sizeof( hiprtDevicePtr ) ) );
			CHECK_ORO( oroMemcpyHtoD(
				(oroDeviceptr)sceneInput.instanceGeometries, geoms, sceneInput.instanceCount * sizeof( hiprtDevicePtr ) ) );

			const float	  offset = 0.3f;
			hiprtFrameSRT frames[5];
			frames[0].translation = make_hiprtFloat3( -0.25f, -offset, 0.0f );
			frames[0].scale		  = make_hiprtFloat3( 1.0f, 1.0f, 1.0f );
			frames[0].rotation	  = make_hiprtFloat4( 0.0f, 0.0f, 1.0f, 0.0f );
			frames[0].time		  = 0.0f;
			frames[1].translation = make_hiprtFloat3( 0.0f, -offset, 0.0f );
			frames[1].scale		  = make_hiprtFloat3( 1.0f, 1.0f, 1.0f );
			frames[1].rotation	  = make_hiprtFloat4( 0.0f, 0.0f, 1.0f, 0.0f );
			frames[1].time		  = 0.35f;
			frames[2].translation = make_hiprtFloat3( 0.25f, -offset, 0.0f );
			frames[2].scale		  = make_hiprtFloat3( 1.0f, 1.0f, 1.0f );
			frames[2].rotation	  = make_hiprtFloat4( 0.0f, 0.0f, 1.0f, Pi * 0.25f );
			frames[2].time		  = 1.0f;
			frames[3].translation = make_hiprtFloat3( 0.0f, offset, 0.0f );
			frames[3].scale		  = make_hiprtFloat3( 1.0f, 1.0f, 1.0f );
			frames[3].rotation	  = make_hiprtFloat4( 0.0f, 0.0f, 1.0f, 0.f );
			frames[3].time		  = 0.0f;
			frames[4].translation = make_hiprtFloat3( 0.0f, offset, 0.0f );
			frames[4].scale		  = make_hiprtFloat3( 1.0f, 1.0f, 1.0f );
			frames[4].rotation	  = make_hiprtFloat4( 0.0f, 0.0f, 1.0f, Pi * 0.5f );
			frames[4].time		  = 1.0f;

			sceneInput.frameCount = 5;
			sceneInput.frameType  = hiprtFrameTypeSRT;
			CHECK_ORO(
				oroMalloc( (oroDeviceptr*)&sceneInput.instanceFrames, sceneInput.frameCount * sizeof( hiprtFrameSRT ) ) );
			CHECK_ORO( oroMemcpyHtoD(
				(oroDeviceptr)sceneInput.instanceFrames, frames, sceneInput.frameCount * sizeof( hiprtFrameSRT ) ) );

			hiprtTransformHeader headers[2];
			headers[0].frameIndex = 0;
			headers[0].frameCount = 3;
			headers[1].frameIndex = 3;
			headers[1].frameCount = 2;
			CHECK_ORO( oroMalloc(
				(oroDeviceptr*)&sceneInput.instanceTransformHeaders,
				sceneInput.instanceCount * sizeof( hiprtTransformHeader ) ) );
			CHECK_ORO( oroMemcpyHtoD(
				(oroDeviceptr)sceneInput.instanceTransformHeaders,
				headers,
				sceneInput.instanceCount * sizeof( hiprtTransformHeader ) ) );

			size_t			  sceneTempSize;
			hiprtDevicePtr	  sceneTemp;
			hiprtBuildOptions options;
			options.buildFlags = hiprtBuildFlagBitPreferFastBuild;
			CHECK_HIPRT( hiprtGetSceneBuildTemporaryBufferSize( ctxt, &sceneInput, &options, &sceneTempSize ) );
			CHECK_ORO( oroMalloc( (oroDeviceptr*)&sceneTemp, sceneTempSize ) );

			CHECK_HIPRT( hiprtCreateScene( ctxt, &sceneInput, &options, &scene ) );
			CHECK_HIPRT( hiprtBuildScene( ctxt, hiprtBuildOperationBuild, &sceneInput, &options, sceneTemp, 0, scene ) );
			CHECK_ORO( oroFree( (oroDeviceptr)sceneTemp ) );
		}

		oroFunction func;
		buildTraceKernelFromBitcode( ctxt, "../common/TutorialKernels.h", "MotionBlurKernel", func );

		u8* pixels;
		CHECK_ORO( oroMalloc( (oroDeviceptr*)&pixels, m_res.x * m_res.y * 4 ) );

		void* args[] = { &scene, &pixels, &m_res };
		launchKernel( func, m_res.x, m_res.y, args );
		writeImage( "07_08_motion_blur.png", m_res.x, m_res.y, pixels );

		CHECK_ORO( oroFree( (oroDeviceptr)sceneInput.instanceGeometries ) );
		CHECK_ORO( oroFree( (oroDeviceptr)sceneInput.instanceFrames ) );
		CHECK_ORO( oroFree( (oroDeviceptr)sceneInput.instanceTransformHeaders ) );
		CHECK_ORO( oroFree( (oroDeviceptr)mesh0.vertices ) );
		CHECK_ORO( oroFree( (oroDeviceptr)mesh0.triangleIndices ) );
		CHECK_ORO( oroFree( (oroDeviceptr)mesh1.vertices ) );
		CHECK_ORO( oroFree( (oroDeviceptr)mesh1.triangleIndices ) );
		CHECK_ORO( oroFree( (oroDeviceptr)pixels ) );

		CHECK_HIPRT( hiprtDestroyGeometry( ctxt, geom0 ) );
		CHECK_HIPRT( hiprtDestroyGeometry( ctxt, geom1 ) );
		CHECK_HIPRT( hiprtDestroyScene( ctxt, scene ) );
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
