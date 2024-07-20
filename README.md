# IntricatePointers
A single-header file written in `C++20` implementing 3 different kinds of smart pointers. The library's API contains the following types:

- **Scope**: A scoped unique pointer intended to resemble `std::unique_ptr`.
- **Ref**: A smart pointer intended to resemble `std::shared_ptr` that implements an intrusive reference counting system.
- **WeakRef**: A weak-referencing smart pointer intended to resemble `std::weak_ptr` and the way it relates to `std::shared_ptr`.
- **UniquePtr**: A typedef for `std::unique_ptr`.
- **SharedPtr**: A typedef for `std::shared_ptr`.
- **WeakPtr**: A typedef for `std::weak_ptr`.

## Basic Usage
### Scope:
``` C++
struct MyStruct
{
    MyStruct(int a, int b) : A(a), B(b) { };
    int A;
    int B;
};

Scope<MyStruct> scopePtr = CreateScope<MyStruct>(21, -21);    // Create a Scope
scopePtr->A; scopePtr->B;                                     // Access the object
scopePtr = nullptr;                                           // Delete the object
```
### Ref:
``` C++
struct MyStruct
{
    MyStruct(int a, int b) : A(a), B(b) { };
    int A;
    int B;
};

Ref<MyStruct> refPtr = CreateRef<MyStruct>(21, -21);          // Create a ref
refPtr->A; refPtr->B;                                         // Access the object
refPtr->RefCount();                                           // Get the pointer's reference count
auto newRef = refPtr;                                         // Increment the reference count by copy-assignment
refPtr = nullptr;                                             // Decrement the reference count
newRef = nullptr;                                             // Now the reference count is 0 and the object is deleted
```
### WeakRef:
``` C++
struct MyStruct
{
    MyStruct(int a, int b) : A(a), B(b) { };
    int A;
    int B;
};

Ref<MyStruct> strongRef = CreateRef<MyStruct>(21, -21);      // Create a ref
WeakRef<MyStruct> weakRef = strongRef;                       // Create a weak ref to 'strongRef'
weakRef->RefCount();                                         // Get the pointer's reference count

// Access the weak ref by locking it.
// This will increment the reference count while the locked ref is in scope to ensure that the resources aren't deleted
if (Ref<MyStruct> lockedRef = weakRef.Lock()) 
{
    lockedRef->A;
    lockedRef->B;
} // Reference count is then decrement when the locked ref falls out of scope

strongRef = nullptr;    // Decrement the reference count (this deletes the object since the reference count is now 0)
// The weak reference would now be expired since there are no strong references to it
weakRef = nullptr;      // Release the weak reference (this only sets the internal pointer to nullptr)
```

## License
IntricateAllocator is licensed under the Apache-2.0 License. See [LICENSE](LICENSE).

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
