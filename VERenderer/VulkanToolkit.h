#pragma once
class VulkanDevice;

class VulkanToolkit
{
public:
    VulkanToolkit(int width, int height, bool enableValidation, std::string windowName);
    ~VulkanToolkit();
    bool shouldCloseWindow();
    void poolEvents();
    double getExecutionTime();
    VulkanDevice* getDevice();

private:
    VulkanDevice* device;
    int windowWidth;
    int windowHeight;
};
