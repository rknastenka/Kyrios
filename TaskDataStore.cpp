#include "pch.h"
#include "TaskDataStore.h"

using namespace winrt;
using namespace winrt::Windows::Data::Json;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Storage;

namespace winrt::WindToDo
{
    JsonObject TaskDataStore::TaskToJson(WindToDo::TaskItem const& task)
    {
        JsonObject obj;
        obj.SetNamedValue(L"id",          JsonValue::CreateStringValue(task.Id()));
        obj.SetNamedValue(L"title",       JsonValue::CreateStringValue(task.Title()));
        obj.SetNamedValue(L"isCompleted", JsonValue::CreateBooleanValue(task.IsCompleted()));
        obj.SetNamedValue(L"createdAt",   JsonValue::CreateStringValue(task.CreatedAt()));
        return obj;
    }

    WindToDo::TaskItem TaskDataStore::JsonToTask(JsonObject const& obj)
    {
        auto item = winrt::make<implementation::TaskItem>(
            obj.GetNamedString(L"id", L""),
            obj.GetNamedString(L"title", L"(untitled)"));

        item.IsCompleted(obj.GetNamedBoolean(L"isCompleted", false));
        item.CreatedAt(obj.GetNamedString(L"createdAt", L""));
        return item;
    }

    IAsyncAction TaskDataStore::SaveTasksAsync(
        IObservableVector<WindToDo::TaskItem> const& tasks)
    {
        // Read existing file first to preserve other keys (e.g. windowRect)
        JsonObject root;
        auto folder = ApplicationData::Current().LocalFolder();
        auto existing = co_await folder.TryGetItemAsync(kFileName);
        if (existing)
        {
            auto file = existing.as<StorageFile>();
            auto text = co_await FileIO::ReadTextAsync(file);
            JsonObject::TryParse(text, root);
        }

        JsonArray arr;
        for (auto const& t : tasks)
            arr.Append(TaskToJson(t));
        root.SetNamedValue(L"tasks", arr);

        auto file = co_await folder.CreateFileAsync(
            kFileName,
            CreationCollisionOption::ReplaceExisting);
        co_await FileIO::WriteTextAsync(file, root.Stringify());
    }

    IAsyncOperation<IObservableVector<WindToDo::TaskItem>>
        TaskDataStore::LoadTasksAsync()
    {
        auto tasks = winrt::single_threaded_observable_vector<WindToDo::TaskItem>();

        auto folder = ApplicationData::Current().LocalFolder();
        auto existing = co_await folder.TryGetItemAsync(kFileName);
        if (existing)
        {
            auto file = existing.as<StorageFile>();
            auto text = co_await FileIO::ReadTextAsync(file);

            JsonObject root;
            if (JsonObject::TryParse(text, root))
            {
                auto arr = root.GetNamedArray(L"tasks", JsonArray{});
                for (uint32_t i = 0; i < arr.Size(); ++i)
                    tasks.Append(JsonToTask(arr.GetObjectAt(i)));
            }
        }

        co_return tasks;
    }

    IAsyncAction TaskDataStore::SaveWindowRectAsync(
        int32_t x, int32_t y, int32_t width, int32_t height)
    {
        JsonObject root;
        auto folder = ApplicationData::Current().LocalFolder();
        auto existing = co_await folder.TryGetItemAsync(kFileName);
        if (existing)
        {
            auto file = existing.as<StorageFile>();
            auto text = co_await FileIO::ReadTextAsync(file);
            JsonObject::TryParse(text, root);
        }

        JsonObject rect;
        rect.SetNamedValue(L"x", JsonValue::CreateNumberValue(x));
        rect.SetNamedValue(L"y", JsonValue::CreateNumberValue(y));
        rect.SetNamedValue(L"w", JsonValue::CreateNumberValue(width));
        rect.SetNamedValue(L"h", JsonValue::CreateNumberValue(height));
        root.SetNamedValue(L"windowRect", rect);

        auto file = co_await folder.CreateFileAsync(
            kFileName,
            CreationCollisionOption::ReplaceExisting);
        co_await FileIO::WriteTextAsync(file, root.Stringify());
    }

    IAsyncOperation<JsonObject>
        TaskDataStore::LoadWindowRectAsync()
    {
        auto folder = ApplicationData::Current().LocalFolder();
        auto existing = co_await folder.TryGetItemAsync(kFileName);
        if (existing)
        {
            auto file = existing.as<StorageFile>();
            auto text = co_await FileIO::ReadTextAsync(file);

            JsonObject root;
            if (JsonObject::TryParse(text, root) &&
                root.HasKey(L"windowRect"))
            {
                co_return root.GetNamedObject(L"windowRect");
            }
        }

        co_return nullptr;
    }
}
