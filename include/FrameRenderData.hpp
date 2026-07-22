#ifndef INCLUDE_FRAME_RENDER_DATA_HPP_
#define INCLUDE_FRAME_RENDER_DATA_HPP_

struct FrameRenderData{
    std::vector<GPUObjectData> object_data_ = {};
    [[nodiscard]] std::span<const GPUObjectData> Objects() const noexcept {
        return object_data_;
    }
    void ClearObjects() noexcept {
        object_data_.clear();
    }
    void Reserve(size_t size) noexcept {
        object_data_.reserve(size);
    }
    void add(const GPUObjectData &object_data) noexcept {
        object_data_.push_back(object_data);
    }
};


#endif // INCLUDE_FRAME_RENDER_DATA_HPP_