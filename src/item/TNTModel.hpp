#pragma once
#include "model/ModelPart.hpp"

class TNTModel {
private:
    ModelPart* model = new ModelPart(0, 0);
public:
    TNTModel(int textureId) {
        float x0 = -8;
        float y0 = -8;
        float z0 = -8;
        float x1 = 8;
        float y1 = 8;
        float z1 = 8;
        this->model->quads.clear();
        NewVertex v1(x0, y0, z0, 0.0f, 0.0f); // 0 0 0
        NewVertex v2(x1, y0, z0, 0.0f, 8.0f); // 1 0 0
        NewVertex v3(x1, y1, z0, 8.0f, 8.0f); // 1 1 0
        NewVertex v4(x0, y1, z0, 8.0f, 0.0f); // 0 1 0
        NewVertex v5(x0, y0, z1, 0.0f, 0.0f); // 0 0 1
        NewVertex v6(x1, y0, z1, 0.0f, 8.0f); // 1 0 1
        NewVertex v7(x1, y1, z1, 8.0f, 8.0f); // 1 1 1
        NewVertex v8(x0, y1, z1, 8.0f, 0.0f); // 0 1 1

        float u0 = ((float)(textureId % 16) * 16.0f) / 256 + (0.01f / 256.0f);
        float u1 = u0 + (16.0f / 256.0f) - ((0.01f / 256.0f) * 2.0f);
        float v0_c = ((float)(textureId / 16) * 16.0f) / 256.0f + (0.01f / 256.0f);
        float v1_c = v0_c + (16.0f / 256.0f) - ((0.01f / 256.0f) * 2.0f);

        float top_u0 = ((float)((textureId+1) % 16) * 16.0f) / 256 + (0.01f / 256.0f);
        float top_u1 = top_u0 + (16.0f / 256.0f) - ((0.01f / 256.0f) * 2.0f);
        float top_v0 = ((float)((textureId+1) / 16) * 16.0f) / 256.0f + (0.01f / 256.0f);
        float top_v1 = top_v0 + (16.0f / 256.0f) - ((0.01f / 256.0f) * 2.0f);

        float btm_u0 = ((float)((textureId+2) % 16) * 16.0f) / 256 + (0.01f / 256.0f);
        float btm_u1 = btm_u0 + (16.0f / 256.0f) - ((0.01f / 256.0f) * 2.0f);
        float btm_v0 = ((float)((textureId+2) / 16) * 16.0f) / 256.0f + (0.01f / 256.0f);
        float btm_v1 = btm_v0 + (16.0f / 256.0f) - ((0.01f / 256.0f) * 2.0f);

        this->model->quads.push_back(Quad({v6, v2, v3, v7}, u1, v1_c, u0, v0_c)); // X-
        this->model->quads.push_back(Quad({v1, v5, v8, v4}, u1, v1_c, u0, v0_c)); // X+

        this->model->quads.push_back(Quad({v6, v5, v1, v2}, btm_u1, btm_v1, btm_u0, btm_v0)); // Y-
        this->model->quads.push_back(Quad({v3, v4, v8, v7}, top_u1, top_v1, top_u0, top_v0)); // Y+

        this->model->quads.push_back(Quad({v2, v1, v4, v3}, u1, v1_c, u0, v0_c)); // Z+
        this->model->quads.push_back(Quad({v5, v6, v7, v8}, u1, v1_c, u0, v0_c)); // Z-
    }

    void generateList() {
        this->model->render(0.0625f);
    }
};