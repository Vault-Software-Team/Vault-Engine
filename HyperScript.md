# HyperScript

## Var keyword

```
var myVar = 5;
```

Defines a variable with a name and a value (depending on the value a generic type will be set to the variable)

---

## Ptr keyword

```
var num = 5;
ptr pNum = num;
```

Gets the "address" (a key of the variable defined in the C++ side) and stores it in a new variable, this is mostly done just for C++ interaction and is kind of unnecessary

---

## Free keyword

```
var playerName = "Player1234";
print(playerName);
free playerName;
```

Deletes the variable, alias of this keyword is `delete`

---

## Func keyword

```
func myPrint(content) {
  print(content);
  print("Used with myPrint");
}
myPrint("Hello, World!");
```

Defines a function, all the arguments are a stack variable so they get freed at the end (obivously)

Aliases: `function`, `fn`

---

## Strict keyword

```
strict var myVar = 5;
```

`strict` locks the variables type to a generic type (number, string, boolean, object)
If you type `myVar = "string"` this'll throw an exception

---

## Global Keyword

```
func defineGlobalVariables(x) {
  global var globalMagicNumber = x;
}

func doSomething() {
  print(globalMagicNumber);
}

defineGlobalVariables("hello!");
doSomething();
```

Global keyword makes the variable in a stack be allocated in a root stack instead of the scope stack

---

## Typedef keyword

```
typedef BoolOrNumber<number | bool>;
```

Creates a hybrid generic type of any (number, bool, string, object) this overrides the strict keyword with its own type checking!

---

## Typevalue keyword

```
typevalue Animals<dog | cat | snake>;
```

Creates a custom type which you can only set the values the type accepts, for string values, `""` are unnecessary

---

## Typeof keyword

```
typedef BoolOrNumber<number | bool>;

typeof<BoolOrNumber> var myVar = true;
```

Creates a variable with a custom type you defined, this overrides the `strict` keyword with it's own custom type checking

---

## Structures and Objects

<p>
Structures in HyperScript are handled kind of different. <br>
In C++ side, Each structure is a string representation of a JSON Object <br>
Example: {"data": "hello"} <br>

Which is then used to create an object, sturctures are just a template for an object

<br>

To create a structure in HyperScript:

Aliases: `template`

```
struct MyObjectTemplate {
  "key": "value",
  "magic": 123,
  "isTrue": false
};
```

To use this structure we created, we create an object:

Aliases: `obj`

```
object myObject<MyObjectTemplate>;

myObject.key = "key123";
print(myObject.key);
```

Since each of the keys in an object is its own variable, you can free it, all of them do get deleted at the end of a scope tho

</p>

---

## References

In HyperScript you can create a `ref` variable, each ref variable has its own counter, if the parent ref is deleted forcefully, you cannot access it anymore from other referneces, they work similarly to pointers in C/C++ but they have a counter which if it reaches 0, gets deleted \
\
Example:

```
ref var myNumber = 123;

func ModifyNumber(refNum) {
  # a new reference called refNum is created, making myNumber's ref_count 2
  refNum = 321; # this modifys myNumber's value
}

ModifyNumber(myNumber);
print(myNumber); # prints 321
```

You can use `ref_count(myRef)` function to get the ref_count of a reference variable. \
\
also note: if you return a reference variable, a value of it gets returned, example:

```
func GetRef() {
  ref var value = "value";
  return value;
}

ref var refString = GetRef();
```

### Why are reference variables useful?

You can modify a variables value without returning by simply passing it as an argument, and also since this language is not garbage collected, it's another way to manage memory, kind of

---

## Arrays

Defining arrays in HyperScript is really easy, they're stored as JSON Objects in C++ side, and all arrays do not have a strict type, meaning a `typeof` or `strict` keyword is useless when making arrays

```
var players = ["Player1", "Player2", "Player3"];

func AddPlayer(player_name) {
  push_back(players, player_name);
}

var player = players<3>;
print(player);
```

You can also get an arrays size using a built-in function: `array_size(myArray)` \
array elements do not work in boolean and math expressions, to pass them, just create a variable containing the arrays element \
This is the same case with functions. \
\
Example:

```
var numbers = [1,2,3,4];
var number = numbers<0>;

if 'number < 2' {
  print("Lower than 2");
}
```

```
func AddOne(number) {
  number += 1;
  return number;
}

var someNumber = 1;
var addedOne = AddOne(someNumber);

if 'addedOne > 1' {
  print("Higher than one");
}
```

---

## While Loop

**THERE ARE NO FOR LOOPS IN THIS LANGUAGE! ONLY WHILE LOOP**

Example:

```
var counter = 0;
var max_count = 10;

while 'counter < 10' {
  print(counter);
  counter += 1;
}
```

Indexing through an array:

```
var myArray = ["hello", "world", "!"];
var size = array_size(myArray);
var i = 0;

while 'i < size' {
  print(myArray<i>);
  i += 1;
}

```
