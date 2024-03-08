const binding = require('./build/Release/ezito-async.node');

/**
 * @param {function} callback 
 */
module.exports.thread = function ( callback ){
    return binding.thread(callback);
}

/**
 * @param {function} callback 
 */
module.exports.queue = function ( callback ){
    return binding.queue(callback);
}


/**
 * @param {function} callback 
 */
module.exports.sync = function ( callback ){
    return binding.sync(callback);
}

