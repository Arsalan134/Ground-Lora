/**
 * @file SD-Card.h
 * @brief SD Card functionality disabled for Ground LoRa Project
 * @author Ground LoRa Project
 * @date 2025
 *
 * SD Card logging has been disabled for this project.
 * All functions are stubbed out to prevent compilation errors.
 */

#pragma once

// SD Card functionality is disabled
// All functions are no-op stubs

// Dummy data structures to prevent compilation errors
struct ControllerData {
  unsigned long timestamp;
  int16_t left_stick_x;
  int16_t left_stick_y;
  int16_t right_stick_x;
  int16_t right_stick_y;
  uint8_t l2_value;
  uint8_t r2_value;
  uint32_t buttons_pressed;
  uint8_t battery_level;
  bool connected;
};

struct GroundTelemetryData {
  unsigned long timestamp;
  int rssi;
  float snr;
  float packet_loss;
  const char* command_sent;
  const char* response_received;
  unsigned long response_time;
  const char* aircraft_status;
  float ground_station_temp;
  uint8_t ground_station_battery;
};

struct CommandData {
  unsigned long timestamp;
  const char* command_type;
  const char* parameters;
  int transmission_power;
  float frequency;
  bool acknowledgment;
  unsigned long execution_time;
};

enum class LogLevel { DEBUG = 0, INFO = 1, WARNING = 2, ERROR = 3, CRITICAL = 4 };

// Dummy SD Logger class with no-op functions
class SDCardLogger {
 public:
  SDCardLogger() {}
  ~SDCardLogger() {}

  // All functions return success but do nothing
  bool begin() { return true; }
  void end() {}
  bool isInitialized() const { return false; }
  const char* getSessionId() const { return "SD_DISABLED"; }

  bool startNewSession() { return true; }
  bool endSession() { return true; }

  bool logControllerData(const ControllerData& data) { return true; }
  bool logControllerCSV(const ControllerData& data) { return true; }
  bool logControllerJSON(const ControllerData& data) { return true; }

  bool logTelemetryData(const GroundTelemetryData& data) { return true; }
  bool logTelemetryCSV(const GroundTelemetryData& data) { return true; }
  bool logTelemetryJSON(const GroundTelemetryData& data) { return true; }

  bool logCommandData(const CommandData& data) { return true; }
  bool logCommandCSV(const CommandData& data) { return true; }
  bool logCommandJSON(const CommandData& data) { return true; }

  bool logEvent(LogLevel level, const char* module, const char* message, const char* details = "") { return true; }
  bool logInfo(const char* module, const char* message, const char* details = "") { return true; }
  bool logWarning(const char* module, const char* message, const char* details = "") { return true; }
  bool logError(const char* module, const char* message, const char* details = "") { return true; }
  bool logCritical(const char* module, const char* message, const char* details = "") { return true; }

  bool exportControllerDataForAnalysis(const char* session_id, const char* output_file) { return true; }
  bool exportTelemetryForPlotting(const char* session_id, const char* output_file) { return true; }
  bool generateSessionSummary(const char* session_id) { return true; }

  bool listLogFiles() { return true; }
  bool readLogFile(const char* filename) { return true; }
  bool deleteLogFile(const char* filename) { return true; }
  bool deleteOldLogs(int days_old) { return true; }

  void printStorageInfo() { Serial.println("📊 SD Card logging is disabled"); }
  unsigned long getTotalSessionTime() { return 0; }
  int getSessionCount() { return 0; }
  void printControllerStats() {}

  void flushAllBuffers() {}
};

// Legacy function stubs
inline void listDir(void* fs, const char* dirname, uint8_t levels) {}
inline void createDir(void* fs, const char* path) {}
inline void removeDir(void* fs, const char* path) {}
inline void readFile(void* fs, const char* path) {}
inline void writeFile(void* fs, const char* path, const char* message) {}
inline void appendFile(void* fs, const char* path, const char* message) {}
inline void renameFile(void* fs, const char* path1, const char* path2) {}
inline void deleteFile(void* fs, const char* path) {}
inline void testFileIO(void* fs, const char* path) {}

// Utility function stubs
inline const char* generateSessionId() {
  return "SD_DISABLED";
}
inline bool initializeSDCard() {
  return false;
}
inline void setupLogDirectories() {}

// Global logger instance (disabled)
extern SDCardLogger sdLogger;
uint8_t battery_level;
bool connected;
}
;

struct GroundTelemetryData {
  unsigned long timestamp;
  int rssi;
  float snr;
  float packet_loss;
  String command_sent;
  String response_received;
  unsigned long response_time;
  String aircraft_status;
  float ground_station_temp;
  uint8_t ground_station_battery;
};

struct CommandData {
  unsigned long timestamp;
  String command_type;
  String parameters;
  int transmission_power;
  float frequency;
  bool acknowledgment;
  unsigned long execution_time;
};

struct SystemEvent {
  unsigned long timestamp;
  String level;   // INFO, WARNING, ERROR, CRITICAL
  String module;  // LORA, PS5, DISPLAY, SYSTEM, COMMAND
  String message;
  String details;
};

// Log Levels
enum class LogLevel { DEBUG = 0, INFO = 1, WARNING = 2, ERROR = 3, CRITICAL = 4 };

class SDCardLogger {
 private:
  bool initialized;
  String current_session_id;
  unsigned long session_start_time;
  std::vector<ControllerData> controller_buffer;
  std::vector<GroundTelemetryData> telemetry_buffer;
  std::vector<CommandData> command_buffer;
  std::vector<SystemEvent> event_buffer;

  // File management
  String getNextLogFileName(const String& prefix);
  bool rotateLogFile(const String& filepath);
  void cleanupOldFiles(const String& prefix, int max_files);
  String formatTimestamp(unsigned long timestamp);

  // Buffer management
  void flushControllerBuffer();
  void flushTelemetryBuffer();
  void flushCommandBuffer();
  void flushEventBuffer();

 public:
  SDCardLogger();
  ~SDCardLogger();

  // Initialization and management
  bool begin();
  void end();
  bool isInitialized() const { return initialized; }
  String getSessionId() const { return current_session_id; }

  // Session management
  bool startNewSession();
  bool endSession();

  // Controller data logging
  bool logControllerData(const ControllerData& data);
  bool logControllerCSV(const ControllerData& data);
  bool logControllerJSON(const ControllerData& data);

  // Ground telemetry logging
  bool logTelemetryData(const GroundTelemetryData& data);
  bool logTelemetryCSV(const GroundTelemetryData& data);
  bool logTelemetryJSON(const GroundTelemetryData& data);

  // Command logging
  bool logCommandData(const CommandData& data);
  bool logCommandCSV(const CommandData& data);
  bool logCommandJSON(const CommandData& data);

  // Event logging
  bool logEvent(LogLevel level, const String& module, const String& message, const String& details = "");
  bool logInfo(const String& module, const String& message, const String& details = "");
  bool logWarning(const String& module, const String& message, const String& details = "");
  bool logError(const String& module, const String& message, const String& details = "");
  bool logCritical(const String& module, const String& message, const String& details = "");

  // Data analysis helpers
  bool exportControllerDataForAnalysis(const String& session_id, const String& output_file);
  bool exportTelemetryForPlotting(const String& session_id, const String& output_file);
  bool generateSessionSummary(const String& session_id);

  // File operations
  bool listLogFiles();
  bool readLogFile(const String& filename);
  bool deleteLogFile(const String& filename);
  bool deleteOldLogs(int days_old);

  // Statistics
  void printStorageInfo();
  unsigned long getTotalSessionTime();
  int getSessionCount();
  void printControllerStats();

  // Force flush all buffers
  void flushAllBuffers();
};

// Legacy functions for backward compatibility
void listDir(fs::FS& fs, const char* dirname, uint8_t levels);
void createDir(fs::FS& fs, const char* path);
void removeDir(fs::FS& fs, const char* path);
void readFile(fs::FS& fs, const char* path);
void writeFile(fs::FS& fs, const char* path, const char* message);
void appendFile(fs::FS& fs, const char* path, const char* message);
void renameFile(fs::FS& fs, const char* path1, const char* path2);
void deleteFile(fs::FS& fs, const char* path);
void testFileIO(fs::FS& fs, const char* path);

// Utility functions
String generateSessionId();
bool initializeSDCard();
void setupLogDirectories();

// Global logger instance
extern SDCardLogger sdLogger;