#include <Engine/Components/Rigidbody2D.hpp>
#include <Editor/GUI/MainGUI.hpp>
#include <icons/icons.h>

namespace Engine::Components {
    void Rigidbody2D::Init() {
        transform = &Scene::Main->EntityRegistry.get<Transform>(entity);
    }

    void Rigidbody2D::OnGUI() {
        DrawComponent<Rigidbody2D>(Scene::Main->EntityRegistry, 40, [&] {
            const char *type_strings[] = {"Static", "Kinematic", "Dynamic"};
            const char *curr_type = type_strings[(int)body_type];
            if (ImGui::BeginCombo("Body Type", curr_type)) {
                for (int i = 0; i < 3; i++) {
                    bool selected = curr_type == type_strings[i];
                    if (ImGui::Selectable(type_strings[i], selected)) {
                        curr_type = type_strings[i];
                        body_type = (BodyType)i;
                    }
                }

                ImGui::EndCombo();
            }
            ImGui::DragFloat("Gravity Scale", &gravity_scale);
        });
    }

    void Rigidbody2D::SetVelocity(float x, float y) {
        if (m_RuntimeBody == nullptr)
            return;
        b2Body *b = (b2Body *)m_RuntimeBody;
        b->SetLinearVelocity(b2Vec2(x, y));
    }

    void Rigidbody2D::SetAngularVelocity(float velocity) {
        if (m_RuntimeBody == nullptr)
            return;
        b2Body *b = (b2Body *)m_RuntimeBody;
        b->SetAngularVelocity(velocity);
    }

    void Rigidbody2D::SetPosition(float x, float y) {
        if (m_RuntimeBody == nullptr)
            return;
        b2Body *b = (b2Body *)m_RuntimeBody;
        b->SetTransform(b2Vec2(x, y), b->GetAngle());
    }

    void Rigidbody2D::Force(float x, float y) {
        if (m_RuntimeBody == nullptr)
            return;
        b2Body *b = (b2Body *)m_RuntimeBody;
        b->ApplyForceToCenter(b2Vec2(x, y), true);
    }

    void Rigidbody2D::Torque(float torque) {
        if (m_RuntimeBody == nullptr)
            return;
        b2Body *b = (b2Body *)m_RuntimeBody;
        b->ApplyTorque(torque, true);
    }
} // namespace Engine::Components
  // namespace Engine::Components