﻿#pragma once

#include <obj.h>

namespace gfx {
	struct AnimatedModelParams;
	class RenderingDevice;
	struct Light3d;
	class AnimatedModel;
	class MdfMaterialFactory;
	using MdfRenderMaterialPtr = std::shared_ptr<class MdfRenderMaterial>;
	using RenderTargetTexturePtr = std::shared_ptr<class RenderTargetTexture>;
}
namespace temple {
	class AasRenderer;
}

class GameSystems;

enum class ShadowType {
	ShadowMap,
	Geometry,
	Blob
};

class MapObjectRenderer {
public:

	MapObjectRenderer(GameSystems& gameSystems, 
		gfx::RenderingDevice& device, 
		gfx::MdfMaterialFactory &mdfFactory,
		temple::AasRenderer &aasRenderer);
	~MapObjectRenderer();

	void RenderMapObjects(int tileX1, int tileX2, int tileY1, int tileY2);
	void RenderObject(objHndl handle, bool showInvisible);

	void RenderObjectHighlight(objHndl handle, const gfx::MdfRenderMaterialPtr &material);

	size_t GetRenderedLastFrame() const {
		return mRenderedLastFrame;
	}
	size_t GetTotalLastFrame() const {
		return mTotalLastFrame;
	}

private:
	GameSystems& mGameSystems;
	gfx::RenderingDevice& mDevice;
	temple::AasRenderer &mAasRenderer;
	ShadowType mShadowType = ShadowType::Blob;
	gfx::MdfRenderMaterialPtr mBlobShadowMaterial;

	size_t mRenderedLastFrame = 0;
	size_t mTotalLastFrame = 0;

	void DrawBoundingCylinder(float x, float y, float z, float radius, float height);

	std::vector<gfx::Light3d> FindLights(LocAndOffsets atLocation, float radius);

	/*
	Same as sin45 incidentally.
	The idea seems to be that the vertical height of the model is scaled
	according to the camera inclination of 45°.
	*/
	static constexpr float cos45 = 0.70709997f;

	bool IsObjectOnScreen(LocAndOffsets &location, float offsetZ, float radius, float renderHeight);
	void RenderMirrorImages(objHndl handle);
	void RenderGiantFrogTongue(objHndl handle);

	void RenderShadowMapShadow(objHndl handle, 
		const gfx::AnimatedModelParams &animParams, 
		gfx::AnimatedModel &model, 
		const gfx::Light3d& globalLight);
	void RenderBlobShadow(objHndl handle, 
		gfx::AnimatedModel &model, 
		gfx::AnimatedModelParams &animParams,
		int alpha);

	objHndl GiantFrogGetGrappledOpponent(objHndl giantFrog);

};