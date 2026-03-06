#pragma once
#include "pch.h"
#include "TaskItem.h"

#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Foundation.Collections.h>

#include <filesystem>
#include <mutex>

namespace winrt::WindToDo
{
    // Handles reading/writing tasks and window geometry to a local JSON file.
    struct TaskDataStore
    {
        static constexpr wchar_t kFileName[] = L"tasks.json";

        static Windows::Foundation::IAsyncAction SaveTasksAsync(
            Windows::Foundation::Collections::IObservableVector<WindToDo::TaskItem> const& tasks);

        static Windows::Foundation::IAsyncOperation<
            Windows::Foundation::Collections::IObservableVector<WindToDo::TaskItem>>
            LoadTasksAsync();

    private:
        static std::filesystem::path GetDataFolder();
        static std::filesystem::path GetDataFilePath();
        static std::wstring ReadFileContents(const std::filesystem::path& path);
        static void WriteFileContents(const std::filesystem::path& path, const std::wstring& text);

        static Windows::Data::Json::JsonObject TaskToJson(
            WindToDo::TaskItem const& task);
        static WindToDo::TaskItem JsonToTask(
            Windows::Data::Json::JsonObject const& obj);

        static std::mutex s_fileMutex;
    };
}
