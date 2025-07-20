#!/bin/bash

# GitHub Actions Local Testing Script
# This script helps validate and test workflows locally

set -e

echo "🔍 GitHub Actions Local Testing Suite"
echo "======================================"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    case $1 in
        "error") echo -e "${RED}❌ $2${NC}" ;;
        "success") echo -e "${GREEN}✅ $2${NC}" ;;
        "warning") echo -e "${YELLOW}⚠️  $2${NC}" ;;
        "info") echo -e "ℹ️  $2" ;;
    esac
}

# Check if we're in a git repository and find the root
if ! git rev-parse --git-dir > /dev/null 2>&1; then
    print_status "error" "Not in a git repository"
    exit 1
fi

# Get the git repository root directory
REPO_ROOT=$(git rev-parse --show-toplevel)
cd "$REPO_ROOT"

print_status "info" "Repository root: $REPO_ROOT"

# Check if .github/workflows directory exists
if [ ! -d ".github/workflows" ]; then
    print_status "error" "No .github/workflows directory found in repository root"
    exit 1
fi

print_status "info" "Found workflows directory"

# 1. Validate YAML syntax
echo ""
echo "1. 📋 Validating YAML Syntax"
echo "----------------------------"

for workflow in .github/workflows/*.yml .github/workflows/*.yaml; do
    if [ -f "$workflow" ]; then
        if command -v yamllint > /dev/null 2>&1; then
            if yamllint "$workflow" > /dev/null 2>&1; then
                print_status "success" "$(basename "$workflow") - YAML syntax valid"
            else
                print_status "error" "$(basename "$workflow") - YAML syntax error"
                yamllint "$workflow"
            fi
        elif python3 -c "import yaml" > /dev/null 2>&1; then
            if python3 -c "import yaml; yaml.safe_load(open('$workflow'))" > /dev/null 2>&1; then
                print_status "success" "$(basename "$workflow") - YAML syntax valid"
            else
                print_status "error" "$(basename "$workflow") - YAML syntax error"
                python3 -c "import yaml; yaml.safe_load(open('$workflow'))"
            fi
        else
            print_status "warning" "$(basename "$workflow") - No YAML validator found (install yamllint or python3-yaml)"
        fi
    fi
done

# 2. Check for common issues
echo ""
echo "2. 🔍 Checking for Common Issues"
echo "--------------------------------"

check_workflow_issues() {
    local file=$1
    local issues=0
    
    # Check for actions versions
    if grep -q "uses:.*@v[0-9]$" "$file"; then
        print_status "warning" "$(basename "$file") - Consider pinning to specific action versions (e.g., @v4.1.2 instead of @v4)"
    fi
    
    # Check for missing required fields
    if ! grep -q "name:" "$file"; then
        print_status "error" "$(basename "$file") - Missing 'name' field"
        ((issues++))
    fi
    
    if ! grep -q "on:" "$file"; then
        print_status "error" "$(basename "$file") - Missing 'on' field"
        ((issues++))
    fi
    
    # Check for shell specification in run commands
    if grep -q "run:" "$file" && ! grep -q "shell:" "$file"; then
        print_status "info" "$(basename "$file") - Consider explicitly specifying shell for run commands"
    fi
    
    return $issues
}

total_issues=0
for workflow in .github/workflows/*.yml .github/workflows/*.yaml; do
    if [ -f "$workflow" ]; then
        check_workflow_issues "$workflow"
        total_issues=$((total_issues + $?))
    fi
done

# 3. Test basic commands locally
echo ""
echo "3. 🧪 Testing Basic Commands Locally"
echo "------------------------------------"

# Test CMake configuration (basic smoke test)
if [ -f "devpilot/CMakeLists.txt" ]; then
    print_status "info" "Testing CMake configuration..."
    cd devpilot
    if cmake -B build-test -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON > /dev/null 2>&1; then
        print_status "success" "CMake configuration successful"
        rm -rf build-test
    else
        print_status "error" "CMake configuration failed"
        ((total_issues++))
    fi
    cd ..
fi

# Test if required dependencies are available
print_status "info" "Checking build dependencies..."

deps_available=0
if command -v cmake > /dev/null 2>&1; then
    print_status "success" "cmake found ($(cmake --version | head -1))"
    ((deps_available++))
else
    print_status "error" "cmake not found"
fi

if command -v gcc > /dev/null 2>&1; then
    print_status "success" "gcc found ($(gcc --version | head -1))"
    ((deps_available++))
else
    print_status "warning" "gcc not found"
fi

if command -v clang > /dev/null 2>&1; then
    print_status "success" "clang found ($(clang --version | head -1))"
    ((deps_available++))
else
    print_status "warning" "clang not found"
fi

if pkg-config --exists sqlite3; then
    print_status "success" "sqlite3 development libraries found"
    ((deps_available++))
else
    print_status "warning" "sqlite3 development libraries not found"
fi

# 4. Summary
echo ""
echo "4. 📊 Summary"
echo "-------------"

if [ $total_issues -eq 0 ]; then
    print_status "success" "No critical issues found!"
else
    print_status "error" "Found $total_issues critical issues"
fi

print_status "info" "Available dependencies: $deps_available"

echo ""
echo "🚀 How to run workflows locally:"
echo "  act                          # Run all push event workflows"
echo "  act pull_request             # Run pull request workflows"
echo "  act -j test                  # Run specific job"
echo "  act --dry-run               # Show what would run without executing"
echo "  act -l                      # List all workflows and jobs"
echo ""
echo "🔧 How to run specific tests:"
echo "  act -j code-quality         # Run code quality checks"
echo "  act -j test                 # Run cross-platform tests"
echo "  act -j coverage             # Run coverage analysis"

exit $total_issues
