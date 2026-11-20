# Portpeek implementation plan

## Repository assessment (2026-11-20)

The repository is an early C17 starter, not a functioning process inspection
tool yet. It contains a CMake executable, a small positional argument parser,
placeholder command handlers, and an empty platform translation unit. The
working tree was clean at assessment time.

### Existing behavior

- `portpeek` and `portpeek --help` print a minimal help heading.
- `portpeek --version` prints version `0.1.0`.
- The parser recognizes `ports`, `inspect PID`, and a numeric port, but those
  handlers only print what they would do.
- Numeric parsing already uses `strtol()` with range and conversion checks.
- Compiler warnings are enabled and treated as errors.

### Missing behavior

- Argument errors do not affect the process exit code; the dispatcher always
  returns success and `main` does not propagate a result.
- `--json`, `--no-color`, `--verbose`, `connections`, `tree`, and
  `inspect --connections` are not implemented.
- There are no process or socket domain models, OS backends, renderers, tests,
  CI workflows, or substantive user/developer documentation.
- `src/platform/platform.c` contains no implementation and is not part of the
  executable target.
- `CMakeLists.txt` was tracked with mixed-case spelling. That happened to work
  on this case-insensitive macOS filesystem but prevents reliable discovery on
  case-sensitive filesystems.

### Baseline verification

- macOS AppleClang configuration and build succeeded using
  `cmake -S . -B /tmp/portpeek-build` and `cmake --build /tmp/portpeek-build`.
- The linker emitted a local warning about a missing Homebrew LLVM search path;
  it is environmental and unrelated to project sources.
- CTest reported that no tests were found.
- No Linux build or macOS native process/socket behavior has been verified.

## Architecture direction

Keep the existing small CLI, but evolve it toward four clear layers:

1. **CLI** parses arguments into a validated command and options, with a
   structured error and the documented exit status.
2. **Application** collects only the information needed for a command and
   turns backend outcomes into success, partial, no-result, or fatal status.
3. **Core** owns portable process, endpoint, socket, warning, and snapshot
   structures. Optional fields carry explicit validity rather than invented
   zero values.
4. **Platform and output** provide native collectors behind a small interface
   and render borrowed domain data as terminal text or schema-versioned JSON.

Use dynamically sized arrays with explicit init/destroy functions. Linux-only
identifiers such as socket inodes remain private to the Linux collector; they
are correlation keys, not public socket identity. Keep platform code in
separate translation units selected by CMake.

## Milestones

1. **Repository assessment and architecture** — this document, exact CMake
   filename, and an agreed layering direction. Done.
2. **CLI and exit codes** — done. The parser now returns a validated command or
   a diagnostic, captures global output options, validates PID/port ranges and
   command arity, and the executable returns usage status 2 for parse failures.
   Placeholder execution returns a status code. CTest covers parser commands,
   invalid input, numeric boundaries, and global options. Discovery and output
   behavior remain intentionally unimplemented.
3. **Portable domain models** — process, endpoint, socket, warning, and
   snapshot ownership contracts, plus unit tests for portable formatting and
   matching.
4. **macOS process collection** — enumerate PIDs and obtain process identity,
   parent, UID, path, command line, start and memory data where available.
5. **macOS socket collection** — inspect process descriptors with libproc,
   identify sockets, extract supported endpoint/protocol/state data, and
   document permission and race limitations.
6. **Linux process collection** — parse procfs with robust dynamic reads,
   explicit missing-field handling, and process identity including start time.
7. **Linux socket collection** — obtain TCP/UDP data from procfs or
   `NETLINK_SOCK_DIAG`, correlate socket inode to descriptors, and report
   incomplete ownership honestly. Choose netlink after validating the needed
   state and namespace behavior; retain procfs as a practical baseline/fallback
   if justified.
8. **Port listing and lookup** — connect collectors to application workflows;
   preserve multiple owners and distinguish UDP state from TCP state.
9. **Output** — stable versioned JSON with correct escaping/nulls and useful
   terminal tables; keep diagnostics on stderr.
10. **Process inspection, connections, and tree** — add targeted workflows,
    connection endpoint rendering, and missing-parent handling.
11. **Integration and reliability** — dynamic-port TCP/UDP/IPv6 fixtures,
    concurrent process changes, permission cases, and sanitizer runs.
12. **CI, docs, and release preparation** — Linux GCC/Clang and macOS Clang
    workflows, architecture/API notes, README, license, and release checklist.
13. **Safe termination (deferred)** — only after stable process identity and
    ownership exist; require confirmation, revalidate identity immediately
    before signaling, and make forced termination explicitly opt-in.

Each milestone will be built and checked before proceeding. A platform feature
will be called verified only after it has been built and exercised on that OS.

## Platform discovery risks

### Linux

- `/proc` visibility, `hidepid`, credentials, namespaces, and process exit can
  hide or invalidate data during collection.
- `/proc/net/*` describes the collector's network namespace; it may not match
  every process namespace. Netlink diagnostics have the same namespace scope.
- Socket tables expose inodes, while `/proc/PID/fd` symlinks expose ownership;
  access restrictions and races mean inode-to-PID mapping can be incomplete or
  ambiguous. Multiple processes may legitimately reference one socket.
- `/proc/PID/stat` has a parenthesized command name, so parsing must locate the
  final `)` rather than split the whole line naively. Clock ticks and boot time
  must be converted carefully for start times.
- IPv4/IPv6 address bytes in procfs use kernel-specific representation and
  require explicit decoding and tests.

### macOS

- `proc_listpids`, `proc_pidinfo`, `proc_pidpath`, and `proc_pidfdinfo` can
  return changing or partial results as processes exit; documented return byte
  counts must be checked against expected structure sizes.
- Socket descriptors can be enumerated with libproc, but endpoint details and
  TCP state availability depend on the descriptor type and SDK structures.
  Inspect the target SDK headers instead of assuming Linux-like fields.
- Process visibility and detailed metadata are subject to permissions and
  hardened runtime/privacy policy; incomplete inspection must be reported.
- Descriptor enumeration followed by socket detail retrieval is inherently
  racy, and PID reuse must be mitigated by associating records with process
  start identity where available.
- There is no promise that a single libproc call supplies every desired
  process/network field; alternative APIs should be evaluated per field.

## Deferred decisions

- Whether Linux socket enumeration uses netlink only or netlink plus procfs
  fallback will be decided after a small working collector and comparison of
  state coverage, ownership correlation, and failure behavior.
- macOS socket state coverage will be based on the installed SDK's actual
  `proc_fdinfo`/`socket_fdinfo` definitions and native experiments.
- Process termination remains out of scope until the identity-revalidation
  design is implemented and reviewed.
