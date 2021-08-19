import React, { useState, useEffect, useRef } from 'react';
import Button from '@material-ui/core/Button';
import TextField from '@material-ui/core/TextField';
import AppBar from '@material-ui/core/AppBar';
import Toolbar from '@material-ui/core/Toolbar';
import Typography from '@material-ui/core/Typography';
import { makeStyles } from '@material-ui/core/styles';
import Grid from '@material-ui/core/Grid';
import axios from 'axios';

const commands = {
	OFF: 0,
  PIANO: 1,
  PONG: 2,
  TWINKLE: 3,
  FADE: 4,
  FILL_LEFT: 5,
  RAINBOW: 6, 
  SPOTIFY: 7,
  SOLID: 8
}

const useStyles = makeStyles((theme) => ({
  root: {
    flexGrow: 1,
  },
  connect: {
    marginLeft: 'auto',
  },
  button: {
    marginTop: "50px",
  }
}));


export default function Main() {

    const classes = useStyles();

    const [command, setCommand] = useState('');
    // const [deviceCache, setDeviceCache] = useState(null);
    const [characteristicCache, setCharacteristicCache] = useState(null);
    const [spotifyStatus, setSpotifyStatus] = useState(false);
    const [songUri, setSongUri] = useState('default');
    
    const baseUrl = "/spotify/"

    if (!navigator.bluetooth) {
        alert('Sorry, your browser doesn\'t support Bluetooth API');
    }

    async function onRequestBluetoothDeviceButtonClick() {
        try {
          console.log(navigator)
          console.log('Requesting any Bluetooth device...');
          await navigator.bluetooth.requestDevice({
            filters: [{name: 'lights'}, {services: ['f468ff61-b23c-419c-b183-e36e12bcdb11']}],
          })
          .then(device => {
            console.log(device);
            // setDeviceCache(device);
            device.addEventListener('gattserverdisconnected',
                handleDisconnection);
            connectDeviceAndCacheCharacteristic(device);
            
          });
        }
        catch(error) {
          console.log('Argh! ' + error);
        }
    }

    function send(data) {
      data = String(data);
    
      if (!data || !characteristicCache) {
        console.log("Data: ", data, "Characteristic: ", characteristicCache);
        return;
      }
    
      data += "\n";
      writeToCharacteristic(characteristicCache, data, 0);
      console.log(data, 'sent');
    }
    
    function writeToCharacteristic(characteristic, string_data, start) {
      if (start >= string_data.length) return;
      characteristic.writeValue(new TextEncoder().encode(string_data.substring(start, (start+20)))).then(foo => {
        writeToCharacteristic(characteristic, string_data, (start+20));
      });
    }
    
    function handleDisconnection(event) {
      let device = event.target;
    
      console.log('"' + device.name +
          '" bluetooth device disconnected, trying to reconnect...');
    
      connectDeviceAndCacheCharacteristic(device);
    }
    
    function connectDeviceAndCacheCharacteristic(device) {
      if (device.gatt.connected && characteristicCache) {
        return Promise.resolve(characteristicCache);
      }
    
      console.log('Connecting to GATT server...');
    
      return device.gatt.connect()
          .then(server => {
            console.log('GATT server connected, getting service...');
    
            return server.getPrimaryService('f468ff61-b23c-419c-b183-e36e12bcdb11');
          })
          .then(service => {
            console.log('Service found, getting characteristic...');
    
            return service.getCharacteristic('dc7d122f-d75e-49a9-ad4f-fc4fda07e848');
          })
          .then(characteristic => {
            console.log('Characteristic found');
            setCharacteristicCache(characteristic);
    
            return characteristicCache;
          });
    }

    function handleSubmit() {
      send(command);
      setCommand('');
    }

    function handleKeyDown(e) {
      if (e.key === 'Enter') {
        handleSubmit();
      }
    }

    function handleOff() {let jsonObject = {"command": commands.OFF}; setSpotifyStatus(false); setSongUri('default'); send(JSON.stringify(jsonObject));}
    function handlePiano() {let jsonObject = {"command": commands.PIANO}; send(JSON.stringify(jsonObject));}
    function handlePong() {let jsonObject = {"command": commands.PONG}; send(JSON.stringify(jsonObject));}
    function handleTwinkle() {let jsonObject = {"command": commands.TWINKLE}; send(JSON.stringify(jsonObject));}
    function handleFade() {let jsonObject = {"command": commands.FADE}; send(JSON.stringify(jsonObject));}
    function handleRainbow() {let jsonObject = {"command": commands.RAINBOW}; send(JSON.stringify(jsonObject));}
    function handleSolid() {let jsonObject = {"command": commands.SOLID, "red": 0, "green": 100, "blue": 255}; send(JSON.stringify(jsonObject));}
    function handleSpotify() {
      if (spotifyStatus) {
        handleOff();
      }
      console.log("Set spotify status to: ", !spotifyStatus);
      setSpotifyStatus(!spotifyStatus);
    }

    function useInterval(callback, delay) {
      const savedCallback = useRef();
    
      useEffect(() => {
        savedCallback.current = callback;
      });
    
      useEffect(() => {
        function tick() {
          savedCallback.current();
        }
    
        let id = setInterval(tick, delay);
        return () => clearInterval(id);
      }, [delay]);
    }

    useInterval(() => {
      if (spotifyStatus) {
        console.log("yay");
        axios.get(baseUrl + 'features/' + songUri).then(response => {
          if (response.data) {
            let data = response["data"];
            setSongUri(data["song_uri"]);
            delete data["song_uri"]
            data["command"] = commands.SPOTIFY;
            send(JSON.stringify(data));
          }
        }).catch(error => {
          console.log(error);
        });
      }
    }, 3000);

    return <div>
      <AppBar position="static">
        <Toolbar className={classes.root}>
          <Typography variant="h6">
            Alex's Light Controller
          </Typography>
          <Button className={classes.connect} color="secondary" variant="contained" onClick={onRequestBluetoothDeviceButtonClick}>Connect</Button>
        </Toolbar>
      </AppBar>

        <Grid container spacing={3}>
          <Grid item xs={4} className={classes.button}>
            <Button size="large" variant="contained" color="secondary" onClick={handleOff}>Turn Off</Button>
          </Grid>
          <Grid item xs={4} className={classes.button}>
            <Button size="large" variant="contained" color="primary" onClick={handlePiano}>Piano</Button>
          </Grid>
          <Grid item xs={4} className={classes.button}>
            <Button size="large" variant="contained" color="primary" onClick={handlePong}>Pong</Button>
          </Grid>
          <Grid item xs={4} className={classes.button}>
            <Button size="large" variant="contained" color="primary" onClick={handleTwinkle}>Twinkle</Button>
          </Grid>
          <Grid item xs={4} className={classes.button}>
            <Button size="large" variant="contained" color="primary" onClick={handleFade}>Fade</Button>
          </Grid>
          <Grid item xs={4} className={classes.button}>
            <Button size="large" variant="contained" color="primary" onClick={handleRainbow}>Rainbow</Button>
          </Grid>
          <Grid item xs={4} className={classes.button}>
            <Button size="large" variant="contained" color={spotifyStatus ? "secondary" : "primary"} onClick={handleSpotify}>
              {spotifyStatus ? "Stop Spotify" : "Spotify"}
            </Button>
          </Grid>
          <Grid item xs={4} className={classes.button}>
            <Button size="large" variant="contained" color="primary" onClick={handleSolid}>Solid</Button>
          </Grid>
        </Grid>
        {/* <TextField onChange={e => setCommand(e.target.value)} value={command} onKeyDown={handleKeyDown}/>
        <Button color="secondary" onClick={handleSubmit}>Submit</Button> */}
      </div>
}
