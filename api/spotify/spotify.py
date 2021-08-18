from flask import request
from flask_restx import Resource, Namespace
import time
import json
import spotipy
import os
from spotipy.oauth2 import SpotifyOAuth

spotify_ns = Namespace("spotify", description="spotify control")

scope = "user-read-currently-playing user-top-read user-read-recently-played user-read-playback-state " \
            "user-modify-playback-state streaming app-remote-control user-library-read"

sp = spotipy.Spotify(auth_manager=SpotifyOAuth(scope=scope, client_id=os.getenv('SPOTIPY_CLIENT_ID'),
                                                client_secret=os.getenv('SPOTIPY_CLIENT_SECRET')))

old_song_uri = None


@spotify_ns.route('/')
class Health(Resource):
    def get(self):
        return 200

@spotify_ns.route('/next_song')
class NextSong(Resource):
    def get(self):
        start = time.time()
        sp.next_track()
        end = time.time()
        return f"This command took {end-start} seconds"


@spotify_ns.route('/previous_song')
class PreviousSong(Resource):
    def get(self):
        start = time.time()
        sp.previous_track()
        end = time.time()
        return f"This command took {end-start} seconds"


@spotify_ns.route('/pause_play')
class PausePlay(Resource):
    def get(self):
        start = time.time()
        if sp.currently_playing().get('is_playing'):
            sp.pause_playback()
        else:
            sp.start_playback()
        end = time.time()
        return f"This command took {end-start} seconds"


@spotify_ns.route('/current_song')
class CurrentSong(Resource):
    def get(self):
        data = sp.currently_playing()
        return data


@spotify_ns.route('/features/<string:old_song_uri>')
class Features(Resource):
    def get(self, old_song_uri):
        data = sp.currently_playing()
        song_uri = data['item'].get('uri')
        print(song_uri, old_song_uri, song_uri == old_song_uri)
        if song_uri == old_song_uri:
            return False
        res = sp.audio_features(tracks=[song_uri])
        tempo, danceability = res[0].get('tempo'), res[0].get('danceability')
        energy, time_signature = res[0].get('energy'), res[0].get('time_signature')
        res = sp.audio_analysis(song_uri)
        segments = [{'start': x.get('start'), 'duration': x.get('duration'), 'loudness': x.get('loudness')} for x in
                    res.get('sections')]
        artist_uri = data['item']['artists'][0]['uri']
        genres = sp.artist(artist_uri).get('genres')
        if len(genres) > 5:
            genres = genres[0:5]
        data = {
            "b": tempo,
            "ts": time_signature,
            "g": genres,
            "song_uri": song_uri
        }
        return data


@spotify_ns.route('/pulse_to_beat')
class PulseToBeat(Resource):
    def post(self):
        data = sp.currently_playing()
        start_time = time.time()
        progress_ms = data.get("progress_ms")/1000
        data = json.loads(request.data)
        light_strand.pulse = bool(data.get("pulse"))
        track_id = data.get('id')
        color = tuple(data.get('color'))
        analysis = sp.audio_analysis(track_id)
        track = analysis.get("track")
        duration = track.get("duration")
        tempo_delta = (60/track.get("tempo")*2)
        curr_song_time = time.time() - start_time + progress_ms
        last_beat = time.time()
        while light_strand.pulse and curr_song_time < duration:
            # print(f"Left: {time.time() - last_beat}  Right: {tempo_delta}")
            if time.time() - last_beat > tempo_delta:
                light_strand.flash_pulse(color)
                last_beat = time.time()
            curr_song_time = time.time() - start_time + progress_ms

