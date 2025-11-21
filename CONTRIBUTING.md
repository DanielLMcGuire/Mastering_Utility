# Contributing Guidelines

Thanks for wanting to contribute! Feel free to contribute to this file as well after reading it.

## Who Can Contribute

* Must have **≥2 years experience in C++ and/or Rust**.
  * **Backend (C++ lib)**: only C++ is required.
  * **Frontend (Rust wrapper and utilities)**: C++ + Rust knowledge required.
* Must understand basic [doxygen syntax](https://www.doxygen.nl/manual/docblocks.html).

## Code Style (if language allows)

* Keep code **readable**; functionality should be clear without comments.
* For **single-line** if/else statements, **do not use brackets**.
* Avoid comments unless something truly cannot be understood.
* AI-generated code is allowed but **must be reviewed** and flagged in the commit description.
* New C++ public members require Doxygen comments.
* Run rustfmt on all rust code before commiting.
```cpp
/**
 * @brief Example Function
 * @param a Parameter a
 * Adds a + 5 if a >= 5
 */
int exampleFunction(int a) {
    if (a >= 5)
        return a + 5;
    else
        return 0;
}
```

## Pull Requests

* **Pull requests only**, no direct commits to main.
* One critical feature per PR.
* Each commit should represent **one change**.
* Write meaningful **commit messages** and **PR descriptions**.

## Testing

* Updating tests is recommended but not strictly enforced yet.
* For **C++ changes**, if `src/frontend/tests.cpp` needs logical updates to pass, do so.
* Pull requests will trigger **automatic CI checks**.

## Issues

* Use the provided **issue templates**.
* Labels are mostly managed automatically; users can mark **bug**, **enhancement**, or **Parity**.

## Dependencies

* Keep external dependencies to a minimum.
* If required, must be fetched via **CMake** or **Cargo**.

## Areas Off-Limits

* **GitHub Actions** workflows are off-limits for contributors without prior discussion with the maintainers (@DanielLMcGuire).

## Review Timeline

* PRs are reviewed as quickly as possible.
* Typical response time ranges from **hours to ~1 week**.
