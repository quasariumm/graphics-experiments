module;

#include <memory>
#include <type_traits>

export module dx_wrapper.core.transform;
import dx_wrapper.external.glm;

export enum class TransformSpace : uint8_t {
	Local, // Call function in local space
	World  // Call function in world space
};

template <typename T>
concept RotationType = std::is_same_v<T, glm::quat> || std::is_same_v<T, glm::vec3>;

export class Transform
{
public:

	Transform() = default;

	//====================================
	// Core
	//====================================

	void MarkDirty() const
	{
		m_localDirty = true;
		m_worldDirty = true;
	}

	//====================================
	// Setters
	//====================================

	/**
	 * @brief Sets the parent of this transform
	 * @param parent The new parent
	 */
	void SetParent(std::shared_ptr<Transform> parent) { m_parent = std::move(parent); }

	/**
	 * @brief Sets the local transform from a matrix
	 * @param matrix The matrix to set the local transform to
	 */
	void SetMatrix(const glm::mat4& matrix);

	/**
	 * @brief Sets the position of the local transform
	 * @param position The new position
	 */
	void SetPosition(const glm::vec3& position);

	/**
	 * @brief Sets the scale of the local transform
	 * @param scale The new scale
	 */
	void SetScale(const glm::vec3& scale);

	/**
	 * @brief Sets the rotation of the local transform
	 * @tparam T @c glm::quat to set from a quaternion, @c glm::vec3 to set from Euler angles
	 * @param rotation The new rotation
	 */
	template <typename T>
		requires RotationType<T>
	void SetRotation(const T& rotation);

	//====================================
	// Getters
	//====================================

	/**
	 * @brief Gets the world matrix of this transform.
	 *		If the transform is dirty, it will update it recursively
	 */
	glm::mat4 GetWorldTransform() const;

	/**
	 * @brief Gets the local matrix of this transform.
	 *		If the transform is dirty, it will update it
	 */
	glm::mat4 GetLocalTransform() const;

	/**
	 * @brief Gets the position of this object.
	 * @param space The space in which the value is taken
	 * @note Position is extracted from the matrix, if it is dirty it is automatically re-calculated
	 */
	glm::vec3 GetPosition(TransformSpace space = TransformSpace::World) const;

	/**
	 * @brief Gets the scale of this object
	 * @param space The space in which the value is taken
	 * @note Scale is extracted from the matrix, if it is dirty it is automatically re-calculated
	 * @warning Non-Uniform scaling can cause issues with certain calculations. Use with caution!
	 */
	glm::vec3 GetScale(TransformSpace space = TransformSpace::World) const;

	/**
	 * @brief Gets the rotation of this object
	 * @tparam T @c glm::quat for quaternion, @c glm::vec3 for Euler angles
	 * @param space The space in which the value is taken
	 * @note Rotation is extracted from the matrix, if it is dirty it is automatically re-calculated
	 */
	template <typename T = glm::quat>
		requires RotationType<T>
	T GetRotation(TransformSpace space = TransformSpace::World) const;

	/**
	 * @brief Gets the forward facing of this object
	 * @param space The space in which the value is taken
	 * @note the forward vector is extracted from the matrix, if it is dirty it is automatically re-calculated
	 */
	glm::vec3 GetForward(TransformSpace space = TransformSpace::World) const;

	/**
	 * @brief Gets the right facing vector of this object
	 * @param space The space in which the value is taken
	 * @note The right vector is extracted from the matrix, if it is dirty it is automatically re-calculated
	 */
	glm::vec3 GetRight(TransformSpace space = TransformSpace::World) const;

	/**
	 * @brief Gets the upwards facing vector of this object
	 * @param space The space in which the value is taken
	 * @note The up vector is extracted from the matrix, if it is dirty it is automatically re-calculated
	 */
	glm::vec3 GetUp(TransformSpace space = TransformSpace::World) const;

private:

	// Dirty flags
	mutable bool m_localDirty = true;
	mutable bool m_worldDirty = true;

	// Matrices
	mutable glm::mat4 m_worldTransform = glm::identity<glm::mat4>();
	mutable glm::mat4 m_localTransform = glm::identity<glm::mat4>();

	// Raw transformation data
	glm::vec3 m_translation{0.0f};
	glm::quat m_rotation = glm::identity<glm::quat>();
	glm::vec3 m_scale{1.0f};

	// Hierarchy
	std::shared_ptr<Transform> m_parent = nullptr;
};

//====================================
// Impl
//====================================

template <typename T>
	requires RotationType<T>
void Transform::SetRotation(const T& rotation)
{
	if constexpr (std::is_same_v<T, glm::quat>)
		m_rotation = rotation;
	else if constexpr (std::is_same_v<T, glm::vec3>)
		m_rotation = glm::quat{rotation};

	MarkDirty();
}

template <typename T>
	requires RotationType<T>
T Transform::GetRotation(TransformSpace space) const
{
	glm::mat4 matrix{};
	if (space == TransformSpace::World)
		matrix = GetWorldTransform();
	else
		matrix = GetLocalTransform();

	const glm::vec3 scale = GetScale(space);

	const glm::mat3 rotationMatrix = {glm::vec3(matrix[0]) / scale.x,
									  glm::vec3(matrix[1]) / scale.y,
									  glm::vec3(matrix[2]) / scale.z};

	if constexpr (std::is_same_v<T, glm::quat>)
		return glm::quat_cast(rotationMatrix);
	else if constexpr (std::is_same_v<T, glm::vec3>)
		return glm::eulerAngles(glm::quat_cast(rotationMatrix));
	// This can't happen because of the concept's bounds
	return 0.f;
}
