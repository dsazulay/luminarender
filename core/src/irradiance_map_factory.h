#pragma once

#include "components/mesh_renderer.h"
#include <glm/mat4x4.hpp>

struct IrradianceMaps
{
    unsigned int cubeMap;
    unsigned int irradianceMap;
    unsigned int prefilterMap;
    unsigned int brdfLUTMap;
};

class IrradianceMapFactory
{
public:
    static IrradianceMaps generateIrradianceMapsFromHDR(unsigned int hdrTexture);
private:
    static unsigned int generateCubeMapTexture(unsigned int hdrTexture, unsigned int captureFBO,
        glm::mat4& captureProjection, glm::mat4 captureViews[], MeshRenderer& mr);
    static unsigned int generateIrradianceTexture(unsigned int captureFBO, unsigned int envCubemap,
        unsigned int captureRBO, glm::mat4& captureProjection, glm::mat4 captureViews[], MeshRenderer& mr);
    static unsigned int generatePrefilterTexture(unsigned int captureFBO, unsigned int envCubemap,
        unsigned int captureRBO, glm::mat4& captureProjection, glm::mat4 captureViews[], MeshRenderer& mr);
    static unsigned int generateLUTTexture(unsigned int captureFBO, unsigned int captureRBO);
};
