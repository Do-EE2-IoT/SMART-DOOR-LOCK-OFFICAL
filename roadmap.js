// Hello world, right now, this file will include entire knowled of Javascript
/**
 * 1. Hoisting
 * Đưa toàn bộ biến được khai báo lên đầu
 * Trong var có hoisting, nghĩa là var chưa khai báo giá trị thì
 * cũng không sao, còn let chưa khai báo sẽ lỗi
 * Var cho phép khai báo lại, còn let thì không
 */

// const { isObject } = require("util");

/**
 * 2. Var name rule
 * Nên đặt tên theo kiểu hisName,...
 * Đối với boolean, phải có is, has, are ở đầu
 * ví dụ : isInitialize ?
 * Đặt tên hàm thì nên sử dụng động từ để hiểu rõ hàm hơn
 */
function getName(firstName, secondName) {
  console.log(firstName + " " + secondName);
}

/**
 * 2. Một class thì nên đặt tên kiểu HelloWorld, hai chữ
 * cái đầu mỗi từ viết hoa để phân biệt
 * Biến private trong class nên có dấu gạch ở trước
 * ví dụ _getName
 * Các hằng số thì sẽ được viết dưới dạng uppercase
 */

/**
 * 3. Kiểu dữ liệu
 *  String : getName = "My name is Do"
 *  Undefined: là giá trị mặc định của biến
 *  Null : được gán vào biến nhưng mà
 *  Kiểu dữ liệu bigint:  đối với một số vô cùng lớn, cần kiểu
 * dữ liệu này, khi viết vd 1111111111111111111111n là số bigint
 */

/**
 *  3. Special : Symbol là kiểu dữ liệu dạng primative data
 */
let myId1 = Symbol("id");
let myId2 = Symbol("id");
console.log(myId1 === myId2); // dù là cùng symbol nhưng vẫn khác nhau
/**
 * Symbol là độc nhất vô nhị, nhưng nếu muốn hay
 * symbol giống nhau khi describe giống nhau ta sử dụng
 * symbol.for("...");
 */

let myId3 = Symbol.for("!");
let myId4 = Symbol.for("!");
console.log(myId3 === myId4);

/**
 *  Ngược lại với phương thức trên, phương thức Symbol.keyFor(symbol)
 *  sẽ trả về giá trị key tương ứng với symbol.
 */
let sym1 = Symbol("id");
let key1 = Symbol.keyFor(sym1);

let sym2 = Symbol.for("name");
let key2 = Symbol.keyFor(sym2);

console.log("key1: ", key1); // => undefined
console.log("key2: ", key2); // => name

/**
 * Vậy ứng dụng của Symbol là gì ?
 * Làm key cho object
 */
const id = Symbol("id");
const obj = {
  [id]: "abc123",
  x: 1,
  y: 2,
};

console.log(obj);
// => {x: 1, y: 2, Symbol(id): "abc123"}

// Symbol không phải là số nên không thể dùng for để duyệt
for (let key in obj) {
  console.log(key);
}
/*
 * x , read more about Symbol https://completejavascript.com/tim-hieu-ve-symbol-trong-javascript/ 
-> Quan trong nhất là ứng dụng symbol trong object để dữ thuộc tính giá trị theo ID ban đầu không bị 
thay đổi
 * y
 */
//---------------------------------------------------------
/**
 * 4. Object , a little bit of memory
 * 4.1. build in object (we build method in object and can take it for
 * many purposes)
 */
const object1 = {
  text1: 2,
  text2: "Hell",
  text3: {
    text4: 4,
    text5: {
      text6: "science",
    },
  },
  fun(numberSmall) {
    console.log(numberSmall);
  },
};

const {
  text1,
  text2,
  text3: {
    text4,
    text5: { text6 },
  },
} = object1;
console.log(text6);
console.log(object1.fun(30));

/**
 * 4.2. Object Prototype
 * 4.2.1. We should understand about object contructor first
 */

// Right here is contructor function for Person Object
function Person(name, age) {
  this.name = name;
  this.age = age;
}

// creat a Person Object, we cannot add anything more into exist object
const myFather = new Person("Dat", 2024 - 1974); // inheritant
const myMother = new Person("Tam ", 2024 - 1977);
document.getElementById(
  "Test Object Constructor"
).innerHTML = ` My father name ${myFather.name} and age is ${myFather.age}`;

// 4.2.2. Object Prototype
// This one allow you add properties to object contructors, also add function and use this overthere
Person.prototype.nationality = "Viet Nam";
document.getElementById(
  "prototype"
).innerHTML = `My mother name ${myMother.name} with age ${myMother.age} live in ${myMother.nationality}`;

//4.2.2 Prototypal inheritant
// Định nghĩa class Animal
class Animal {
  constructor(name) {
    this.name = name;
  }

  // Phương thức trong class Animal
  eat() {
    console.log(`${this.name} is eating.`);
  }
}

// Định nghĩa class Cat kế thừa từ class Animal
class Cat extends Animal {
  constructor(name, breed) {
    super(name); // Gọi constructor của class cha
    this.breed = breed;
  }

  // Phương thức trong class Cat
  meow() {
    console.log(`${this.name} says meow!`);
  }
}

// Tạo một đối tượng mới từ class Cat
const myCat = new Cat("Tom", "Siamese");

// Gọi phương thức từ class Animal
myCat.eat(); // Output: Tom is eating.

// Gọi phương thức từ class Cat
myCat.meow(); // Output: Tom says meow!

// myCat kế thừa từ prototype của cả Animal và Cat

//4.3. Type of operator

let num = 10;
let str = "Hello";
let bool = true;
let obje = { key: "value" };
let arr = [1, 2, 3];
let func = function () {};

console.log(typeof num); // Output: number
console.log(typeof str); // Output: string
console.log(typeof bool); // Output: boolean
console.log(typeof obje); // Output: object
console.log(typeof arr); // Output: object (arrays are objects in JavaScript)
console.log(typeof func); // Output: function

/**
 *  5. Type casting
 * 5.1. Implicit Type casting : Ép kiểu ngầm dịnh
 * Đây là tự động ép kiểu, không cần can thiệp từ người dùng.
 */
let numb = 10; // Kiểu dữ liệu number
let stri = "The number is: " + num; // Ép kiểu ngầm định từ number sang string để nối chuỗi
console.log(stri); // Output: The number is: 10
let result = "3" * 2; // Ép kiểu ngầm định từ string sang number để thực hiện phép nhân
console.log(result); // Output: 6

//5.2. Explicit Type Casting ( ép kiểu rõ ràng)
let strNumber = "10"; // Kiểu dữ liệu string
let numbt = parseInt(strNumber); // Ép kiểu rõ ràng từ string sang integer
console.log(numbt); // Output: 10

//5.3. Type conversion và Type Coercion cũng đều là ép kiểu dữ liệu

/**
 * 6. Data Structure
 * 6.1. Trong JavaScript, một "index collection" có thể được hiểu là một tập hợp các
 *  giá trị được truy cập thông qua các chỉ mục số nguyên, giống như một mảng.
 *  Dưới đây là một ví dụ minh họa:
 */
// Khởi tạo một index collection
const indexCollection = {
  0: "Giá trị 1",
  1: "Giá trị 2",
  2: "Giá trị 3",
};

// Truy cập các giá trị thông qua chỉ mục
console.log(indexCollection[0]); // Output: Giá trị 1
console.log(indexCollection[1]); // Output: Giá trị 2
console.log(indexCollection[2]); // Output: Giá trị 3

// Thêm một giá trị mới
indexCollection[3] = "Giá trị 4";

// Truy cập giá trị mới
console.log(indexCollection[3]); // Output: Giá trị 4

/**
 * 6.2. Array and typed Array: là các dạng dữ liệu đặc biệt
 */

// Tạo một Uint8Array với độ dài 4 phần tử
const uint8Array = new Uint8Array(4);

// Gán giá trị cho các phần tử của mảng
uint8Array[0] = 10;
uint8Array[1] = 20;
uint8Array[2] = 30;
uint8Array[3] = 40;

// Truy cập và in giá trị các phần tử
for (let i = 0; i < uint8Array.length; i++) {
  console.log(uint8Array[i]); // Output: 10 20 30 40
}

/**
 * 6.3. Tương tác với array
 */
// To String
const Language = ["english", "Korean", "vietnamese"];

console.log(Language.toString());
// output (Tạo ra một đoạn string cách nhau bởi dấu phẩy)
// "english","Korean", "VietNam"

//join (Creat array and mid is ...)
console.log(Language.join(" x "));
//output: english x Korean x vietnamese

// Pop()  - delete the final element of array
// push('a', 'b',..) -- add defined numnber of element
// shift - delete the first element
shift = Language.shift(); // return first element of array
console.log(shift);
console.log(Language);

// Unshift()
//-> Add one element to the first of array
Language.unshift("Hello World"); // return new length of array
console.log(Language);

// Splicing
// Delete the number of el in array
// with begin element and return element deleted
// And it have ability to insert one +++ element
console.log(Language.splice(1, 1, " mix "));
console.log(Language);

// concat -> connect array
const add = ["1", "2"];
testConcat = Language.concat(add);
console.log(testConcat);
console.log(Language); // not change origin array

// Slice -> Cut some element of array ( copy )
Hex = Language.slice(1);
console.log(Hex);
console.log(Language); // Not effect origin array

/**
 *  6.4. method with array -> Important
 */
var cityVietNam = [
  {
    area: "Bac Ninh",
    length: "100km",
    money: 100,
  },
  {
    area: "Hai Phong",
    length: "200km",
    money: 200,
  },
  {
    area: "Ho Chi Minh",
    length: "300km",
    money: 300,
  },
  {
    area: "Bac Ninh",
    length: "200km",
    money: 100,
  },
];

//6.4.1. forEach ( all element) // always can contact with array
cityVietNam.forEach((el) => {
  console.log(el.area); // output -> so all element inside object
  console.log(el.money); // output -> so all element inside object
});

// 6.4.2. Every -> return Boolean
// kiem tra tung value trong phan tu, neu
// mot phan tu bi sai, return false immediately
var isSmaller200 = cityVietNam.every((el, index) => {
  console.log(index);
  return el.money <= 500;
});
console.log(isSmaller200);

// 6.4.3. Some -> return Boolean
// when it meet only one element -> true
var isSmaller200 = cityVietNam.some((el, index) => {
  console.log(index);
  return el.money >= 500;
});
console.log(isSmaller200); // output true false
/**
 * every(): Hàm này trả về true nếu tất cả các phần tử trong mảng đều thỏa mãn 
 * điều kiện được đặt ra trong hàm callback và false nếu có ít nhất một phần tử không thỏa mãn điều kiện. 
 * Nếu bất kỳ lần gọi hàm callback nào trả về false, thì every() ngay lập tức kết thúc và trả về false mà
 *  không kiểm tra các phần tử còn lại.

some(): Ngược lại, hàm này trả về true nếu ít nhất một phần tử trong mảng thỏa mãn điều kiện được đặt ra trong hàm
 callback và false nếu không có phần tử nào thỏa mãn điều kiện. Nếu bất kỳ lần gọi hàm callback nào trả về true, thì some() 
 ngay lập tức kết thúc và trả về true mà không kiểm tra các phần tử còn lại.
 */

// 6.4.4.  find () -> to check that in array have element
// same to things the user finding
// if yes it will return that object, or none

var findBacNinh = cityVietNam.find((el) => {
  return el.area === "Bac Ninh";
});

//console.log(`find element -> ${findBacNinh}`); // cannot
console.log(findBacNinh); // return true object -> only one object

// 6.4.5 filter -> find all object same to our idea
var findBacNinh = cityVietNam.filter((el) => {
  return el.area === "Bac Ninh";
});
console.log(findBacNinh); // output : all element have area = BacNinh

// 6.4.5 Array Map -> create new array
// return -> thay đổi giá trị thep cái ta mong muốn
// cần copy , kết hợp tạo những element mới hơn
// element gốc thì dùng cái này
// Kế thừa mảng cũ và khởi tạo mảng mới
var newCity = cityVietNam.map((el, index, originArray) => {
  return {
    areaNew: el.area,
    moneyNew: el.money + 1,
    width: `Here width ${el.length}`,
    originArray: originArray, // mảng gốc
  };
});
console.log(newCity);

// 6.4.6. Array Reduce
// khi muốn nhận về giá trị duy nhất , VS tổng số tuổi của các object
// Chúng ta có thể sử dụng vòng lặp để tính tổng chẳng hạn
// tại sao chúng ta cần reduce() ? -> ngắn gọn và đơn giản hơn

var totalMoney = cityVietNam.reduce((el, currenValue) => {}, 0); // 0 is origin value -> NEED to learn More

//6.5. Keyed Collections

/**
 * 8. Loop and interation
 * we have for, while, do-while like normal
 * -> What diffirent over here ???
 */
// 8.1. For --- in
const myInfo = {
  name: "Nguyen Van Do",
  age: 20,
  address: "Bac Ninh",
};

for (var key in myInfo) {
  console.log(`${key} and it value: ${myInfo[key]}`);
  // key and it respective value
  // output: name and its value : Nguyen Van Do
}

// 8.2. for --- of
for (var value of Language) {
  console.log(value);
} // -> this is better way for for( i = 0; i < n; i++){...}
//----------------------------------------------------------------------------------
// // here is some kind of other knowledge
// example1 : labeled statement
//  i think it look like assembly language
// outerIf: if (condition1) {
//   console.log('Điều kiện 1 đúng');
//   innerIf: if (condition2) {
//       break outerIf; // Nhảy ra khỏi khối điều kiện bên ngoài
//   }
//   console.log('Ngoài khối điều kiện 2');
// }
// Example 2: thow statement, try, catch, finally
function divide(x, y) {
  try {
    if (y === 0) {
      throw new Error("Divide by zero error");
    }
    return x / y;
  } catch (error) {
    console.error("Caught an error:", error.message);
    // Optionally, handle the error or rethrow it
    // throw error; // Rethrow the same error
    // throw new Error('Custom error message'); // Throw a new error
  } finally {
    console.log("Finally block is always executed");
  }
}

console.log(divide(10, 2)); // Output: 5
console.log(divide(10, 0)); // Output: Caught an error: Divide by zero error
//         Finally block is always executed
console.log("Điều kiện 2 đúng");

//------------------------------------------------------------------------------------

/**
 * 9: Function
 */

// 9.1. IIFEs Immediately Invoked Function Expression
// This is a private function, every variable and
// anything overthere dont effect outside of code
(function () {
  console.log("NOW NOW");
})();
(() => {
  console.log("NOW NOW!");
})();
// Would you like to hide data ?
var app = (function () {
  const cars = []; // private
  return {
    // từ đây mới là object
    // sorry, nhìn cái này hơi lạ nên mình khó hiểu
    add(car) {
      cars.push(car);
    },
    delete(index) {
      cars.splice(index, 1);
    },
    get(index) {
      return cars[index];
    },
  };
})();
console.log(typeof app); // object
// tại sao lại là object nhỉ , hiểu như này nhé
// viết như trên thì return thay cho bằng thôi mà
// nên nó như kiểu một cách viết khác của object
// right here, it return 2 method and run that method immendiately

// Scope and Function Stack
// global
var globalMessage = "This is global";

// code block, we can see trong ngoac la code block
// Lexical Scope, biến trong hàm lồng bên trong có thể truy cập biến hàm ngoài
{
  const age = 18;
  console.log(age); // output = 18
}
// console.log(age); // error undefined -> dangerous, cannot run
// khi ta gọi một hàm, một phạm vi mới được tạo ra
// Cứ có biến nằm ngoài hàm, trong hàm có thể truy cập được.
function FUN() {
  var o;
  function FUn() {
    console.log(o); // truy cập được biến o
  }
  FUn();
}

// Khi nào một biến bị xóa ra khỏi bộ nhớ ?
// -> load trang thì biến sẽ bị xóa
// block code => Khối thực thi xong, khối tự động bị xóa khỏi bộ nhớ

/**9.2. Strict Mode
 * Ví dụ trường hợp mình khai báo biến trong function nhưng quên
 * khai báo var, let, khi đó ngoài function scope
 * Biến vẫn in ra -> Gây rò rỉ thông tin
 * Do đó, tại trên đỉnh của file Js, viết string
 * "use strict" tại dòng đầu tiên là xong
 * Ví dụ như những thứ mà mình không cho sửa đổi
 * Ví dụ
 */
("use strict");
const student = Object.freeze({
  // read only
  fullName: "Nguyen Van Do",
});
student.fullName = " Nguyen Van Duc"; // we accidentally change it value
// but console not notify for me
// so we must use 'use strict'
// Cấm một số từ khóa biến đặt trùng với từ khóa của JS gây hiểu nhầm

console.log(student);
//--------------------------------------------------------------------
/**
 * 9.3. Using " this keyword"
 * This nằm ở đâu thì nó thuộc về cái đó
 */
const smartPhone = {
  // smartPhone == this
  // Attribute
  name: "Samsung A22",
  color: "Black",
  weight: 100,

  // Property
  logThis() {
    console.log(this); // this belong to smartphone Object
  },

  objChild: {
    // this == objChild
    he: "he",
    methodChild() {
      console.log(this);
    },
  },
};
console.log(smartPhone.logThis());
// Out put{name: 'Samsung A22', color: 'Black',
//weight: 100, logThis: ƒ}

// Use like this we can have prototype
function Car(name, color, weight) {
  this.name = name; // -> Object : Car.name = "...." khi gọi từ cái khác
  this.color = color;
  this.weight = weight;
  var tryHard = " Try test speed";
  this.run = function () {
    // the way to add function
    console.log(`${this.name} right now is ${tryHard}`);
  };
}
/**
 * Write like above same to -> viết kiểu hàm tạo
 * constructor thì được cái là thằng này linh hoạt hơn nhiều
 * Có thể viết kiểu OOP chứ không bị cố định như object
 * Car{
 * name : "....",
 * color : "..." }
 */

// this == Mercedess333
const mercedesS333 = new Car("Mercerdes", "red", 100);
mercedesS333.run();

// Try with DOM
const button = document.querySelector("button");
button.onclick = function clickButton() {
  console.dir(this.innerText);
};

// 9.4. Using Chrome Dev Tool

/**
 * 10. Asynchronous Javascript
 */
// 10.1. Callback
// Yeah, the way to use Callback is master
// Right now I will learn how to create Callback
Array.prototype.map2 = function (callbackhere) {
  var output = [];
  for (var i = 0; i < this.length; i++) {
    var result = callbackhere(this[i], i);
    output.push(result);
  }
  return output;
};
var books = ["Math", "English", "Science"];

// var htmls = books.map((el) => {
//   return `<h5> ${el} </h5> `;
// });
// // console.log(htmls.join(''));
// console.log(htmls);
// console.log(htmls.join(""));
// var HTML = document.body.querySelector(".here");
// HTML.innerHTML = `${htmls.join('')}`;

// create new callback function
var htmls = books.map2((el, index) => {
  return `<h5> ${el} </h5> `;
});
var HTML = document.body.querySelector(".here");
HTML.innerHTML = `${htmls.join("")}`;
// map 2 called base on the mumber of index

// 10.1.x why callback hell is Problem ?
// callback hell
// Pyramid of doom
// code is difficult to read, and fix

//10.2. Promise
// when new created, function called
var promise = new Promise(function (resolve, reject) {
  resolve("then thứ nhất"); //Khi gọi resolve cùng với data được xử lý,
  // data sẽ nhảy vào trong then thứ nhất
  //reject("Failed"); // nếu call reject(...) thì data được nhảy vào catch
  // chỉ được dùng catch hoặc resolve, không được dùng cả hai
});

promise
  .then((data) => {
    console.log(data + "2"); //
    return new Promise(function (resolve) {
      setTimeout(() => resolve(5), 3000);
    }); // data then thứ 2 cho ra thì sẽ nhảy xuống then tiếp theo
    // cho tới khi hết return thì thôi
  })
  .then((data) => {
    return data + 1;
  })
  .then((data) => {
    console.log("Successful " + data);
  })

  .catch((Data) => {
    console.log(Data);
  })
  .finally(() => {
    console.log("final here");
  });

// Print 1,2,3,4,5,6,..
// Promise ở trước sẽ chạy then của thằng tiếp theo
function sleep(ms) {
  return new Promise(function (resolve) {
    // viết như thế này thường đi cùng với logic ở bên trong
    setTimeout(resolve, ms);
  });
}

sleep(1000)
  .then((data) => {
    console.log(1);
    return sleep(1000);
  })
  .then((data) => {
    console.log(2);
    return sleep(1000);
  })
  .then((data) => {
    console.log(3);
    return sleep(1000);
  })
  .then((data) => {
    console.log(4);
    return sleep(1000);
  });

// Method with promise
var promiseMethod = Promise.resolve("1234231");

promiseMethod
  .then((data) => {
    console.log(data);
    return Promise.resolve("5678920");
  })
  .then((data) => {
    console.log(data);
  });

// Chạy hai Promise song song
var promise1 = new Promise((resolve, reject) => {
  setTimeout(resolve("5000_1"), 4000);
});

var promise2 = new Promise((resolve, reject) => {
  setTimeout(resolve("5000_2"), 5000);
});
// All promise is done, it will go into then
var allPromise = Promise.all([promise1, promise2]).then((data) => {
    var result1 = data[0];
    var result2 = data[1];
    return result1.concat(result2);
});

allPromise
.then(data => {
  console.log(data + " ALL  PROCESSING");
})
