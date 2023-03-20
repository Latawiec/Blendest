#pragma once

#include <string_view>

namespace Model {

namespace FieldName {
// High-level
const std::string_view Settings = "settings";
const std::string_view Connection = "connection";

// Settings
const std::string_view ThreadCount = "threadCount";
const std::string_view BlenderDirPath = "blenderPath";

// Connection
const std::string_view HostServerAddress = "hostAddress";
const std::string_view HostServerPort = "hostPort";

}

}