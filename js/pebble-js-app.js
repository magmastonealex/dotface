Pebble.addEventListener('showConfiguration', function(e) {
  // Show config page
  Pebble.openURL('http://magmastonealex.github.io/dotface/');
});

Pebble.addEventListener('webviewclosed',
  function(e) {
    var configuration = JSON.parse(decodeURIComponent(e.response));
    console.log('Configuration window returned: ', JSON.stringify(configuration));
  }
);