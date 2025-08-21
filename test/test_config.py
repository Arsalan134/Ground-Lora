# 🧪 Test Configuration for Ground-Lora

# 🎯 Test filtering configuration
# This file defines which tests to run in different scenarios

# 🛠️ Core functionality tests (always run)
CORE_TESTS = ["test_utilities", "test_lora", "test_ps5"]

# 🛡️ Safety tests (critical for flight systems)
SAFETY_TESTS = ["test_safety", "test_integration"]

# 🖥️ UI and display tests
UI_TESTS = ["test_display"]

# 🚀 System tests
SYSTEM_TESTS = ["test_main"]

# 🔧 Quick test suite (for rapid development)
QUICK_TESTS = ["test_utilities", "test_safety"]

# 🏆 Full test suite (for CI/CD and releases)
FULL_TESTS = CORE_TESTS + SAFETY_TESTS + UI_TESTS + SYSTEM_TESTS
