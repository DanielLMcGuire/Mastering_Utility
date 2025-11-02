#include "MasteringUtility.h"
#include "cxx.h"
#include <memory>
#include <string>

std::unique_ptr<MasteringUtility> new_mastering_utility() {
    return std::make_unique<MasteringUtility>();
}
