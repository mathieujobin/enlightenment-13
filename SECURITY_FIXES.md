# Security and Bug Fixes for Enlightenment DR 13.3

This document summarizes the critical security vulnerabilities and bugs that were fixed to modernize Enlightenment DR 13.3 for compatibility with modern X.org servers and to address serious security issues.

## Security Vulnerabilities Fixed

### 1. Command Injection Vulnerabilities (CRITICAL)

**Affected Files:** `misc.c`, `actions.c`

**Issue:** Multiple functions used `sprintf()` to construct shell commands with unsanitized user input (Theme_Path), then executed them with `system()`. This could allow arbitrary command execution if Theme_Path contained shell metacharacters.

**Original Vulnerable Code:**
```c
sprintf(s, "rm -rf %s", Theme_Path);
system(s);
```

**Fix:** Replaced with safe recursive directory removal function `rmrf()` that uses system calls directly without shell interpretation:
- Created `rmrf()` function in `file.c` that uses `unlink()`, `rmdir()`, and recursion
- Updated `EExit()`, `Do_Exit()`, `Do_Restart()`, and `Do_Restart_Theme()` to use `rmrf()`
- No shell metacharacter expansion possible

**Impact:** Prevents arbitrary command execution vulnerability.

### 2. Buffer Overflow Risks

**Affected Files:** `misc.c` (lines 50, 95)

**Issue:** Used `sizeof(VERSION)` instead of `strlen(VERSION)` in malloc calculations. Since VERSION is a string pointer, `sizeof(VERSION)` returns the pointer size (4-8 bytes) rather than the string length.

**Original Buggy Code:**
```c
s = malloc(272 + sizeof(VERSION));  // sizeof pointer, not string!
sprintf(s, "Enlightenment version %s\n...", VERSION);
```

**Fix:**
```c
s = malloc(272 + strlen(VERSION) + 1);  // correct string length + null terminator
if (!s) return;  // added null check
sprintf(s, "Enlightenment version %s\n...", VERSION);
```

**Impact:** Prevents potential buffer overflow and memory corruption.

## Critical Logic Bugs Fixed

### 3. Root Window Handling (root.c)

**Line 29 Bug:** Variable assignment error
```c
// Before (WRONG):
im_w = desk.bg_height[desk.current];  // assigned height to width variable!

// After (CORRECT):
im_h = desk.bg_height[desk.current];  // correctly assign to height variable
```

**Added Validation:**
- Null check for image loading
- Validation of XGetGeometry return value
- Null check for pixmap creation
- Error messages for debugging

### 4. X Connection Issues (main.c)

**Improvements:**
- Added root window validation after `DefaultRootWindow()`
- Added `XGetWindowAttributes()` check to verify root window accessibility
- Added `XSync()` calls before and after `XSelectInput()` for immediate error detection
- Added diagnostic output showing successful connection

This addresses issues with modern X.org servers (2018+) that may not expose the root window properly, especially with compositing.

### 5. File Operations Bugs (file.c)

**Line 79 - Wrong Variable Check:**
```c
// Before (WRONG):
names[i] = malloc(...);
if (!names) { ... }  // checking wrong pointer!

// After (CORRECT):
names[i] = malloc(...);
if (!names[i]) { ... }  // check the allocated element
```

**Lines 144, 157 - Inverted stat() Logic:**
```c
// Before (WRONG):
if (!stat(s, &st))  // stat returns 0 on SUCCESS, so this fails when file exists!
    return 0;

// After (CORRECT):
if (stat(s, &st) != 0)  // properly check for error
    return 0;
```

**Line 118 - Inefficient File Copy:**
```c
// Before (SLOW):
unsigned char buf[1];  // 1 byte buffer!
while (fread(buf, 1, 1, f))
    fwrite(buf, 1, 1, ff);

// After (FAST):
unsigned char buf[4096];  // 4KB buffer
size_t n;
while ((n = fread(buf, 1, sizeof(buf), f)) > 0)
    fwrite(buf, 1, n, ff);
```

**Line 169 - Deprecated getcwd() Usage:**
```c
// Before (NON-PORTABLE):
return getcwd(NULL, -1);  // -1 is not portable

// After (PORTABLE):
char *buf;
size_t size = 1024;
buf = malloc(size);
if (!buf) return NULL;
while (getcwd(buf, size) == NULL) {
    if (errno != ERANGE) {
        free(buf);
        return NULL;
    }
    size *= 2;
    buf = realloc(buf, size);
    if (!buf) return NULL;
}
return buf;
```

### 6. Memory Leak (lists.c)

**Lines 65-74 - Use After Free:**
```c
// Before (MEMORY LEAK):
while (node) {
    KillEWin(node->win);
    node = node->next;  // accessing freed memory!
    free(node);
}

// After (CORRECT):
while (node) {
    next_node = node->next;  // save next pointer first
    KillEWin(node->win);
    free(node);
    node = next_node;  // use saved pointer
}
```

## Testing

All modified files were syntax-checked and compile successfully with gcc.

## Compatibility

All fixes maintain backward compatibility with the original 1998 behavior while adding necessary safety checks for modern systems.

## Summary

- **Security Vulnerabilities Fixed:** 2 critical (command injection, buffer overflow)
- **Logic Bugs Fixed:** 6 critical bugs
- **Files Modified:** 7 files (root.c, main.c, misc.c, actions.c, file.c, file.h, lists.c)
- **Lines Changed:** ~98 insertions, ~26 deletions
- **Backward Compatibility:** Maintained

These fixes are essential for running Enlightenment DR 13.3 on modern systems safely.
