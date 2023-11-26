#pragma once

class Camera;
class Material;

class NormalVisualizerPass 
{
public:
    NormalVisualizerPass();
    void render();
    void camera(Camera* camera);

private:
    void renderNormalVectorOfEntity();

    Camera* m_camera{};
    Material* m_material;
};
