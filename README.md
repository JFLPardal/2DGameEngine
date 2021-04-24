# 2DGameEngine
Simple 2D game engine that uses SDL, GLM, Dear ImGUI and Lua, based on an online course

## Main Features
* Entity-Component-Sytem architecture
* Event system
* Test suite (this is an extra effort I am doing to practice testing software) 

## Things to Improve:
- [ ] Visibility of arguments when adding a component - Right now the way to show the arguments when adding a component is by either going to that component's file or by hovering over the component type which displays its arguments informaadastion. This info needs to be added manually and when we want to change the default value of a parameter we need to update it in 2 places. Ideally updating the default value would update the text that displays it, and there would be a better way to show the programmer what the arguments for that component are.

- [ ] Data structures used for Tags and Groups  - to ensure fast access when using a tag to get an entity id and using an entity to get a tag, we have 2 data structures that  contain the same data but with different representations. This is not a problem per se, but the data structures used are not efficient (at least some of them). This same problem occurs for groups.     

- [ ] Use pre compiled headers - compilation is getting slower than it needs to be and the more that is added to the engine the worse it's going to get (and harder it will be to implement this) 

- [ ] Data validation - some data is assumed to exist and there is no code to protect the program if that data is not present. Some examples are - an invalid filepath for a texture, checking if an entity has a given tag when that tag does not exist and places where we are using pointers that are not all being null verified
