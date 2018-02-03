#pragma once
class TransformationManager;
class TransformationJoint
{
public:
    TransformationManager * target;
    TransformationManager * frame;

    TransformationJoint(TransformationManager * t, TransformationManager * f) {
        target = t;
        frame = f;
    }
};

struct TransformStruct {
public:
    glm::vec3 position;
    glm::vec3 size;
    glm::quat orientation;    
    TransformStruct();
    TransformStruct(glm::vec3 iposition);
    TransformStruct(glm::vec3 iposition, glm::quat iorientation);
    TransformStruct(glm::vec3 iposition, glm::quat iorientation, glm::vec3 isize);
    TransformStruct(glm::vec3 iposition, glm::vec3 isize);
    TransformStruct(glm::quat iorientation);
    TransformStruct(glm::quat iorientation, glm::vec3 isize);
    TransformStruct& operator*=(const TransformStruct& rhs)
    {
        position += rhs.position;
        orientation = orientation * rhs.orientation;
        size *= rhs.size;
        return *this;
    }
    friend TransformStruct operator*(TransformStruct& lhs, const TransformStruct& rhs)
    {
        return TransformStruct(lhs.position + rhs.position, lhs.orientation * rhs.orientation, lhs.size * rhs.size);
    }
};

class TransformationManager
{
public:
    TransformationManager();
    TransformationManager(glm::vec3 iposition);
    TransformationManager(glm::vec3 iposition, glm::quat iorientation);
    TransformationManager(glm::vec3 iposition, glm::quat iorientation, glm::vec3 isize);
    TransformationManager(glm::vec3 iposition, glm::vec3 isize);
    TransformationManager(glm::quat iorientation);
    TransformationManager(glm::quat iorientation, glm::vec3 isize);
    ~TransformationManager();

    void copyTo(TransformationManager* v);
    void copyFrom(TransformationManager* v);
     
    volatile bool needsUpdate = false;

    glm::vec3 getPosition();
    glm::vec3 getSize();
    glm::quat getOrientation();

    void setPosition(glm::vec3 value);
    void setSize(glm::vec3 value);
    void setOrientation(glm::quat value);

    void translate(glm::vec3 value);
    void scale(glm::vec3 value);
    void rotate(glm::quat value);

    glm::mat4 getWorldTransform();
    glm::mat4 getInverseWorldTransform();
    glm::mat4 getRotationMatrix();

    void addJoint(TransformationJoint* joint) {
        joints.push_back(joint);
        updateJoints();
    }

    void removeJoint(TransformationJoint* joint) {
        joints.erase(std::remove(joints.begin(), joints.end(), joint));
        updateJoints();
    }

    std::vector<TransformationJoint*> getJoints() {
        return joints;
    }

    void updateJoints();
    
    TransformStruct getStruct()
    {
        return TransformStruct(position, orientation, size);
    }

private:

    glm::vec3 position;
    glm::vec3 size;
    glm::quat orientation;

    std::vector<TransformationJoint*> joints = std::vector<TransformationJoint*>();
};
