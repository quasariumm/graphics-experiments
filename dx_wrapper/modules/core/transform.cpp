module;

module dx_wrapper.core.transform;

void Transform::SetMatrix(const glm::mat4& matrix)
{
	m_localTransform = matrix;
	
	// Decompose
	glm::vec3 skew{};
	glm::vec4 perspective{};
	glm::decompose(matrix, m_scale, m_rotation, m_translation, skew, perspective);
}

void Transform::SetPosition(const glm::vec3& position)
{
	m_translation = position;
	MarkDirty();
}

void Transform::SetScale(const glm::vec3& scale)
{
	m_scale = scale;
	MarkDirty();
}

glm::mat4 Transform::GetWorldTransform() const
{
	if (!m_worldDirty && !m_localDirty)
		return m_worldTransform;
	m_worldDirty = false;

	if (!m_parent)
	{
		m_worldTransform = GetLocalTransform();
	}
	else
	{
		const glm::mat4 rootMatrix = m_parent->GetWorldTransform();
		m_worldTransform		   = rootMatrix * GetLocalTransform();
	}

	return m_worldTransform;
}

glm::mat4 Transform::GetLocalTransform() const
{
	if (!m_localDirty)
		return m_localTransform;
	m_localDirty = false;
	m_localTransform =
			glm::translate(glm::mat4(1.0f), m_translation) * glm::mat4_cast(m_rotation) * glm::scale(glm::mat4(1.0f), m_scale);
	return m_localTransform;
}

glm::vec3 Transform::GetPosition(const TransformSpace space) const
{
	if (space == TransformSpace::World)
		return GetWorldTransform()[3];
	return GetLocalTransform()[3];
}

glm::vec3 Transform::GetScale(const TransformSpace space) const
{
	glm::mat4 matrix{};
	if (space == TransformSpace::World)
		matrix = GetWorldTransform();
	else
		matrix = GetLocalTransform();

	return {glm::length(glm::vec3(matrix[0])), glm::length(glm::vec3(matrix[1])), glm::length(glm::vec3(matrix[2]))};
}

glm::vec3 Transform::GetForward(const TransformSpace space) const
{
	if (space == TransformSpace::World)
		return -GetWorldTransform()[2];
	return -GetLocalTransform()[2];
}

glm::vec3 Transform::GetRight(const TransformSpace space) const
{
	if (space == TransformSpace::World)
		return GetWorldTransform()[0];
	return GetLocalTransform()[0];
}

glm::vec3 Transform::GetUp(const TransformSpace space) const
{
	if (space == TransformSpace::World)
		return GetWorldTransform()[1];
	return GetLocalTransform()[1];
}
