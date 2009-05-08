#include "stdafx.h"
#include <process.h>
#include <crtdbg.h>
#include "RenderThread.h"
#include "RenderMgr.h"
#include "VertexDecl.h"

namespace Helix {

const int			STACK_SIZE =				16*1024;	// 16k
const int			NUM_SUBMISSION_BUFFERS	=	2;
bool				m_renderThreadInitialized =	false;
bool				m_renderThreadShutdown =	false;
bool				m_inRender =				false;
HANDLE				m_hThread	=				NULL;
HANDLE				m_startRenderEvent	=		NULL;
HANDLE				m_rendererReady =			NULL;
HANDLE				m_hSubmitMutex =			NULL;
IDirect3DDevice9 *	m_D3DDevice =				NULL;

struct RenderData
{
	D3DXMATRIX		worldMatrix;
	std::string		meshName;
	std::string		materialName;
	RenderData *	next;
};

int				m_submissionIndex = 0;
RenderData *	m_submissionBuffers[NUM_SUBMISSION_BUFFERS];
D3DXMATRIX		m_viewMatrix[NUM_SUBMISSION_BUFFERS];
D3DXMATRIX		m_projMatrix[NUM_SUBMISSION_BUFFERS];

// ****************************************************************************
// Thread function
// ****************************************************************************
void RenderThreadFunc(void *data);

// ****************************************************************************
// ****************************************************************************
inline bool AcquireMutex()
{
	DWORD result = WaitForSingleObject(m_hSubmitMutex,INFINITE);
	_ASSERT(result == WAIT_OBJECT_0);

	return result == WAIT_OBJECT_0;
}

// ****************************************************************************
// ****************************************************************************
inline bool ReleaseMutex()
{
	DWORD result = ::ReleaseMutex(m_hSubmitMutex);
	_ASSERT(result);

	return result != 0;
}

// ****************************************************************************
// ****************************************************************************
bool RenderThreadReady()
{
	DWORD result = WaitForSingleObject(m_rendererReady,INFINITE);
	_ASSERT(result == WAIT_OBJECT_0);

	return result == WAIT_OBJECT_0;
}

// ****************************************************************************
// ****************************************************************************
void SetDevice(IDirect3DDevice9* dev)
{
	_ASSERT(dev != NULL);
	m_D3DDevice = dev;
}
// ****************************************************************************
// ****************************************************************************
void InitializeRenderThread()
{
	_ASSERT(m_renderThreadInitialized == false);
	m_renderThreadInitialized = true;

	m_startRenderEvent = CreateEvent(NULL, false, false,"RenderStartEvent");
	_ASSERT(m_startRenderEvent != NULL);

	m_rendererReady = CreateEvent(NULL, false, false, "RenderEndEvent");
	_ASSERT(m_rendererReady != NULL);

	m_submissionIndex = 0;
	for(int i=0;i<NUM_SUBMISSION_BUFFERS; i++)
	{
		m_submissionBuffers[i] = NULL;
	}

	m_hSubmitMutex = CreateMutex(NULL,false,"RenderSubmit");

	// Now create the thread
	m_hThread = (HANDLE)_beginthread( Helix::RenderThreadFunc, STACK_SIZE, NULL );
//	_ASSERT(m_hThread != 1L);

}

// ****************************************************************************
// ****************************************************************************
bool GetRenderThreadShutdown()
{
	return m_renderThreadShutdown;
}

// ****************************************************************************
// ****************************************************************************
void ShutDownRenderThread()
{
	m_renderThreadShutdown = true;

	// Wait for the thread to exit
	DWORD result = WaitForSingleObject(m_rendererReady,INFINITE);
	_ASSERT(result == WAIT_OBJECT_0);

	CloseHandle(m_startRenderEvent);
	CloseHandle(m_rendererReady);

	// Thread safety first!
	AcquireMutex();

	for(int i=0;i<NUM_SUBMISSION_BUFFERS;i++)
	{
		RenderData *obj = m_submissionBuffers[i];
		while(obj != NULL)
		{
			RenderData *nextObj = obj->next;
			delete obj;
			obj = nextObj;
		}
	}
	ReleaseMutex();
	CloseHandle(m_hSubmitMutex);
}

// ****************************************************************************
// ****************************************************************************
void RenderScene()
{
	DWORD result = SetEvent(m_startRenderEvent);
	_ASSERT(result != 0);
}

// ****************************************************************************
// I want to have the renderer take meshes and materials separately.
// But in the case where I am rendering an instance, I will pull the name of
// the material out of the mesh.
// ****************************************************************************
void SubmitInstance(Instance &inst)
{
	// Thread safety first
	AcquireMutex();

	// Create an object which holds our rendering information
	RenderData *obj = new RenderData;

	// Save the world matrix
	obj->worldMatrix = inst.GetWorldMatrix();

	// Save the name of the mesh
	obj->meshName = inst.GetMeshName();
	
	// Get the name of the material
	Mesh *mesh = MeshManager::GetInstance().GetMesh(obj->meshName);
	obj->materialName = mesh->GetMaterialName();

	// Setup the object
	obj->next = m_submissionBuffers[m_submissionIndex];

	// Put it at the head of the list
	m_submissionBuffers[m_submissionIndex] = obj;

	// Thread safety first
	ReleaseMutex();
}

// ****************************************************************************
// ****************************************************************************
void SubmitViewMatrix(D3DXMATRIX &mat)
{
	AcquireMutex();
	m_viewMatrix[m_submissionIndex] = mat;
	ReleaseMutex();
}

// ****************************************************************************
// ****************************************************************************
void SubmitProjMatrix(D3DXMATRIX &mat)
{
	AcquireMutex();
	m_projMatrix[m_submissionIndex] = mat;
	ReleaseMutex();
}

// ****************************************************************************
// ****************************************************************************
void RenderThreadFunc(void *data)
{
	while(!GetRenderThreadShutdown())
	{
		DWORD result = SetEvent(m_rendererReady);
		_ASSERT(result != 0);

		result = WaitForSingleObject(m_startRenderEvent,INFINITE);
		_ASSERT(result == WAIT_OBJECT_0);

		// Thread safety first
		AcquireMutex();

		int renderSubmissionIndex = m_submissionIndex;
		m_submissionIndex = (m_submissionIndex + 1 ) % NUM_SUBMISSION_BUFFERS;

		ReleaseMutex();

		IDirect3DDevice9 *device = m_D3DDevice;
		_ASSERT(device);

		// Initialize our render
		device->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,0xff000000, 1.0f, 0L );
		device->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);
		device->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
		device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

		// Start rendering
		device->BeginScene();

		// Setup camera parameters
		Helix::ShaderManager::GetInstance().SetSharedParameter("WorldView",m_viewMatrix[renderSubmissionIndex]);
		Helix::ShaderManager::GetInstance().SetSharedParameter("Projection",m_projMatrix[renderSubmissionIndex]);

		// Go through all of our render objects
		RenderData *obj = m_submissionBuffers[renderSubmissionIndex];
		while(obj)
		{
			// Set the material parameters
			Material *mat = MaterialManager::GetInstance().GetMaterial(obj->materialName);
			mat->SetParameters();

			// Get the effect
			Shader *shader = ShaderManager::GetInstance().GetShader(mat->GetShaderName());
			ID3DXEffect *effect = shader->GetEffect();

			// Set the vertex format
			VertexDecl &decl = shader->GetDecl();

			UINT numPasses;
			effect->Begin( &numPasses, D3DXFX_DONOTSAVESTATE );
			effect->BeginPass( 0 );

			effect->CommitChanges();

			// Set the vb/ib
			Mesh *mesh = MeshManager::GetInstance().GetMesh(obj->meshName);
			device->SetStreamSource(0,mesh->GetVertexBuffer(),0,decl.VertexSize());
			device->SetIndices(mesh->GetIndexBuffer());
			device->SetVertexDeclaration(decl.GetDecl());
			device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,mesh->NumVertices(),0,mesh->NumTriangles());

			effect->EndPass();
			effect->End();

			// Next
			obj=obj->next;
		}

		device->EndScene();
		device->Present(NULL,NULL,NULL,NULL);

		// Delete all our render objects
		obj = m_submissionBuffers[renderSubmissionIndex];
		while(obj != NULL)
		{
			RenderData *nextObj = obj->next;
			delete obj;
			obj = nextObj;
		}

		m_submissionBuffers[renderSubmissionIndex] = NULL;

	}
}

} // namespace Helix
