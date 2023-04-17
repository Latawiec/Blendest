#include "public/Model/Blender/Settings.hpp"

namespace Model {
namespace Blender {

void Settings::ResetHandle(const std::string& newBlenderPath)
{
    _handle = Model::Blender::Handle::TryCreateHandle(newBlenderPath);
}

std::optional<Model::Blender::Handle>& Settings::GetHandle()
{
    return _handle;
}

void Settings::SetThreadsCount(int threadsCount)
{
    _threadsCount = threadsCount;
}

int Settings::GetThreadsCount() 
{
    return _threadsCount;
}

}
}