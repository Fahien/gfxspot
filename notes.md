# Notes

- [x] gfx::draw function should take as parameter a collection of vertices to render as points or lines primitives.
- [x] Another gfx::draw function will take as parameter a mesh, or a collection of meshes, or a scene, to render as triangles primitives.
- [x] In order to optimize performances, these functions should not create vulkan resources needed to draw. These resources should be created upfront, or asynchronously and used when ready.
- [x] The draw functions will find vulkan resources associated with each argument to be rendered in their specific Renderer objects.
- [x] The renderer objects can be released when they are not needed anymore, for example when the current scene changes.
- [x] A GLTF scene should be rendered.
- [ ] Draw some text.
- [ ] Make a sphere programmatically.
- [ ] Draw a fan of cards.
