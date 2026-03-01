#pragma once
#include "pch.h"
#include "TaskItem.h"

#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Foundation.Collections.h>

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

        static Windows::Foundation::IAsyncAction SaveWindowRectAsync(
            int32_t x, int32_t y, int32_t width, int32_t height);

        static Windows::Foundation::IAsyncOperation<Windows::Data::Json::JsonObject>
            LoadWindowRectAsync();

    private:
        static Windows::Data::Json::JsonObject TaskToJson(
            WindToDo::TaskItem const& task);
        static WindToDo::TaskItem JsonToTask(
            Windows::Data::Json::JsonObject const& obj);
    };
}
