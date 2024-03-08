const fs = require('fs');
    const path = require('path');

    (function deleteFolderRecursive(directoryPath) {
        try {
            if (fs.existsSync(directoryPath)) {
                fs.readdirSync(directoryPath).forEach((file, index) => {
                  const curPath = path.join(directoryPath, file);
                  if (fs.lstatSync(curPath).isDirectory()) {
                   // recurse
                    deleteFolderRecursive(curPath);
                  } else {
                    // delete file
                    fs.unlinkSync(curPath);
                  }
                });
                fs.rmdirSync(directoryPath);
              }
        } catch (error) {
            
        }
    })("./build");