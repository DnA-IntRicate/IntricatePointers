# IntricatePointers
A single-include header file written in `C++20` implementing 3 different kinds of smart pointers.

- **Scope**: A scoped unique pointer intended to resemble `std::unique_ptr`.
- **Ref**: A smart pointer intended to resemble `std::shared_ptr` that implements an intrusive reference counting system.
- **WeakRef**: A weak-referencing smart pointer intended to resemble `std::weak_ptr` and the way it relates to `std::shared_ptr`.

## License
IntricateAllocator is licensed under the Apache 2.0 License. See [LICENSE](LICENSE).

```
Copyright 2024 Adam Foflonker

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```
