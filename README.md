## Installation


```bash

npm install ezito-async

```


```js

const ea = require('ezito-async');

new Promise(res => { 
    setTimeout(() => {
        console.log("Promise end")
        res(1);
    }, 1000);
});
console.log("---end---");


// output frist ---end---
// and output -> Promise end
```


```js
const ea = require('ezito-async');
new Promise(res => { 
    setTimeout(() => {
        console.log("Promise end") // output second
        res(1);
    }, 1000);
});
console.log("---end--"); // output frist
var res = ea.sync(async result => {
    await new Promise(res => {
        result(1);
        setTimeout(() => {
            res(1);
        }, 2000);
    })
});
console.log(res) // output 1 called result in sync
console.log("---end--") // output the end
```