# DevPilot — Local AI Codebase Navigator (C++ Core)

[![CI](https://github.com/ars1mm/devpilot/actions/workflows/ci.yml/badge.svg)](https://github.com/ars1mm/devpilot/actions/workflows/ci.yml)
[![Release](https://github.com/ars1mm/devpilot/actions/workflows/release.yml/badge.svg)](https://github.com/ars1mm/devpilot/actions/workflows/release.yml)
[![codecov](https://codecov.io/gh/ars1mm/devpilot/branch/main/graph/badge.svg)](https://codecov.io/gh/ars1mm/devpilot)

## 🧠 Overview

**DevPilot** is a local-first developer tool that helps engineers understand, navigate, and refactor large codebases using natural language, code graph analysis, and AI-powered summarization — all backed by a high-performance **C++ engine**.

---

## 💡 Problem Statement

Modern developers are overwhelmed when working on large, legacy, or unfamiliar codebases. They struggle with:

- Understanding call graphs and dependencies
- Tracking function usage and definitions across files
- Refactoring confidently without breaking things
- Manually generating summaries or comments

**DevPilot** solves this by combining static code parsing, symbol indexing, and AI-driven Q&A into one offline-first tool that plugs into any IDE.

---

## MVP Feature List (v0.1)

### ✅ Core Functional Features

- [ ] **Project Indexing Engine** (C++):
  - Parse source code using Tree-sitter or Clang
  - Extract function/class definitions, usages, imports, comments
  - Store symbol graph and file relations in SQLite or LMDB

- [ ] **Natural Language Q&A Interface**:
  - Query codebase using plain English (via CLI or GUI)
  - Examples:
    - "Where is `processOrder()` used?"
    - "What files depend on `UserService`?"
    - "Summarize the file `checkout.ts`"

- [ ] **Call Graph Visualizer** (Tauri/TS GUI):
  - Show all functions that call/called a given function
  - Interactive flowchart with arrows and file links

- [ ] **File/Function Summarizer**:
  - Use local LLM (Code LLaMA via llama.cpp or GPT-based fallback)
  - Summarize function/class in simple terms
  - Optional inline comment generation

- [ ] **VSCode + Neovim Plugin**:
  - Keybind to “Ask DevPilot”
  - Hover to show summary or usage map

---

## 🗺️ First Month Roadmap

### Week 1: C++ Parser & Indexing Prototype
- Set up core C++ engine
- Use Tree-sitter/Clang to extract symbols and function calls
- Build LMDB or SQLite-based store for fast lookup

### Week 2: CLI + Search Interface
- Build simple CLI to:
  - Query symbol usage
  - Print call tree
  - Search definitions by keyword or path
- Add basic JSON API layer for GUI

### Week 3: Natural Language Q&A + Summarization
- Integrate llama.cpp (local Code LLaMA GGUF)
- Create prompt template: "Summarize this function:"
- Expose `devpilot ask "..."` command
- Test Q&A + summaries offline

### Week 4: GUI + Plugins (VSCode/NeoVim)
- Build Tauri GUI: show files, call graph, Q&A box
- Create minimal VSCode extension with:
  - DevPilot panel
  - Ask popup
  - Hover summaries
- Prepare MVP demo repo

---

## 🧱 Code Architecture

```text
DevPilot
│
├── core/                  # C++ symbol graph + parser engine
│   ├── parser/            # Tree-sitter or Clang bindings
│   ├── graph/             # Call graph + symbol table builder
│   ├── storage/           # LMDB/SQLite access layer
│   └── engine.cpp         # Main indexing + query logic
│
├── cli/                   # Rust or C++ CLI wrapper
│   └── main.cpp           # devpilot ask / search / summary
│
├── llm/                   # LLM integration
│   ├── prompt_templates/  # Few-shot prompts for summarizing code
│   └── llama_runner.cpp   # Call local model via llama.cpp
│
├── gui/                   # Tauri (Rust + TS) GUI
│   ├── src/components/    # React/Svelte components
│   ├── api/               # Communicates with CLI/json
│   └── diagrams/          # Call graph visualizer (D3/Cytoscape)
│
├── plugins/
│   ├── vscode/            # VSCode extension
│   └── nvim/              # Lua plugin for Neovim
│
└── data/
    ├── snippets.db        # Stored user snippets
    └── index.db           # LMDB or SQLite-based symbol DB
