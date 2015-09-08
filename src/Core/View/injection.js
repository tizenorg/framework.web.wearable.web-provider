// set javascript objects for Web APIs of Tizen appwidget
var appTizenObject = 0;
if (typeof window.tizen == 'undefined') {
    window.tizen = new Object();
    window.tizen.appwidget = new Object();
} else {
    appTizenObject = 1;
}

// For future, only window.appwidget will be used
window.appwidget = new Object();


window.appwidget.fireReadyEvent = function () {
    // If every functionalities of appwidget are initialized, fire appwidget ready event
    var readyevent = document.createEvent("CustomEvent");
    readyevent.initCustomEvent("appwidgetready", true, true);
    document.dispatchEvent(readyevent);
};

// these are functions for overriding standard javascript functions regarding event
var original_addEventListener = window.addEventListener;
var original_removeEventListener = window.removeEventListener;

// this variable is responsible to keep information of appwidget evetns
var appWidgetEvents = {};

// define event structure for appwidget
window.AppWidgetEventInfo = function(event, callback) {
    this.event = event;
    this.callback = callback;
};

// this variable is responsible to keep information of box
var appWidgetContexts = {};

// define box info for appwidget (e.g. box id, box instance id)
window.AppWidgetContextInfo = function(key, value) {
    this.key = key;
    this.value = value;
};

window.addEventListener = function(event, callback, capture) {
    var e = event.toLowerCase();
    if (typeof appWidgetEvents[e] != 'undefined') {
        appWidgetEvents[e].callback = callback;
    } else {
        original_addEventListener.call(window, event, callback, capture);
    }

    if (e == 'appwidgetready') {
        // fire ready event to content
        setTimeout(window.appwidget.fireReadyEvent, 0);
    }
};

window.removeEventListener = function(event, callback, capture) {
    var e = event.toLowerCase();
    if (typeof appWidgetEvents[e] != "undefined") {
        appWidgetEvents[e].callback = "null";
    } else {
        original_removeEventListener.call(window, event, callback, capture);
    }
};

window.appwidget.reload = function() {
    window.location.href = "box://reload";
};

window.appwidget.changePeriod = function(period) {
    switch (arguments.length) {
    case 0:
        window.location.href = "box://change-period";
        break;
    case 1:
        window.location.href = "box://change-period?period=" + period;
        break;
    default:
        window.location.href = "box://change-period";
        break;
    }
};

window.appwidget.launchBrowser = function(url) {
    window.location.href = "box://launch-browser?url=" + url;
};

window.appwidget.scrollStart = function() {
    window.location.href = "box://scroll-start";
};

window.appwidget.scrollStop = function() {
    window.location.href = "box://scroll-stop";
};

window.appwidget.sendMessageToBox = function(message) {
    window.location.href = "box://send-message-to-box?message=" + message;
};

window.appwidget.sendMessageToGbar = function(message) {
    window.location.href = "box://send-message-to-pd?message=" + message;
};

window.appwidget.getBoxId = function() {
    return appWidgetContexts["box-id"].value;
};

window.appwidget.getBoxInstanceId = function() {
    return appWidgetContexts["instance-id"].value;
};

var webprovider = {
    // define specific function for registering appwidget event
    registerAppWidgetEvent: function(event) {
        return (appWidgetEvents[event] = new AppWidgetEventInfo(event, "null"));
    },

    // register resources for synchronous APIs like getBoxId, getBoxInstanceId
    registerAppWidgetContextInfo: function(key, value) {
        return (appWidgetContexts[key] = new AppWidgetContextInfo(key, value));
    },

    // define specific function for firing registered appwidget event
    fireAppWidgetEvent: function(event, data) {
        // this is called by web-provider, which is native code
        if (typeof appWidgetEvents[event] != 'undefined') {
            setTimeout(function() {
                appWidgetEvents[event].callback(data);
            }, 0);
            console.log("fire appwidget event: " + event);
        } else {
            console.log("unknown appwidget event: " + event);
        }
    },
};

// register custom events for appwidget
webprovider.registerAppWidgetEvent("gbarmessage");
webprovider.registerAppWidgetEvent("boxmessage");

// These objects will be deprecated soon
if (!appTizenObject) {
    window.tizen.appwidget.reload = window.appwidget.reload;
    window.tizen.appwidget.changePeriod = window.appwidget.changePeriod;
    window.tizen.appwidget.launchBrowser = window.appwidget.launchBrowser;
    window.tizen.appwidget.scrollStart = window.appwidget.scrollStart;
    window.tizen.appwidget.scrollStop = window.appwidget.scrollStop;
}

// fire ready event explicitly
window.appwidget.fireReadyEvent();
