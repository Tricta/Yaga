#include <atomic>
#include <cstdint>

class artMethod{
public:
    void* declaring_class_;
    std::atomic<std::uint32_t> access_flags_;

    uint32_t dex_method_index_;
    uint16_t method_index_;

    union {
        uint16_t hotness_count_;
        uint16_t imt_index_;
    };

    struct PtrSizedFields {
        void* data_;
        void* entry_point_from_quick_compiled_code_;
    } ptr_sized_fields_;
};