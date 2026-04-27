# Beat Singer

Beat Singer is now a Quest standalone Beat Saber mod packaged as a `.qmod`.
It displays timed lyrics during gameplay on Quest 2, Quest 3, and Quest Pro.

This branch targets Beat Saber Quest `1.40.8_7379`, the latest publicly listed
moddable Quest version at the time of the Quest port. Quest 1 is not supported.
The old PC/BSIPA DLL and companion Bottle/Heroku service have been removed.

## Version Targets

The project pins the latest dependency set that is publicly listed for Beat
Saber Quest `1.40.8_7379`:

- `bs-cordl 4008.0.0`
- `beatsaber-hook 6.4.2`
- `SongCore 1.1.26`
- `BSML 0.4.55`
- `custom-types 0.18.3`
- `paper2_scotland2 4.7.0`

Newer QPM packages exist for some of these projects, but they target newer
Beat Saber/codegen stacks and are intentionally not used until Quest mod support
lists a newer moddable game version.

## Lyrics

BeatSinger loads lyrics in this order by default:

1. `lyrics.json` in the current custom song folder.
2. `lyrics.srt` in the current custom song folder.
3. Optional configured HTTP provider, disabled by default.

The HTTP provider keeps the legacy BeatSinger lyric hash so existing hosted
collections can be adapted, but there are no hardcoded Musixmatch, Heroku, or
credential-backed lookups.

### JSON Format

```json
[
  { "text": "Never gonna give you up", "startTimeSeconds": 10.0, "endTimeSeconds": 11.1 },
  { "text": "Never gonna let you down", "time": 11.24 }
]
```

The legacy `time` and `end` field names are still accepted.

### SRT Format

```srt
1
00:00:22,791 --> 00:00:26,229
Never gonna give you up.
Never gonna let you down.

2
00:00:30,023 --> 00:00:32,272
Never gonna run away...
And desert you.
```

## Configuration

Config is stored in Quest mod storage at
`/sdcard/ModData/com.beatgames.beatsaber/Configs/BeatSinger.json`.

- `enabled`
- `displayDelay`
- `hideDelay`
- `verticalOffset`
- `textScale`
- `providerOrder`
- `httpEnabled`
- `httpBaseUrl`
- `httpTimeoutSeconds`

The BSML settings page is registered under `Beat Singer`.

## Build

Install the Quest modding toolchain, Android NDK, and QPM, then run:

```sh
qpm restore
qpm s build
qpm s qmod
```

To copy to a connected headset and watch logs:

```sh
qpm s copy
qpm s logcat -self
```

The generated `.qmod` is intended for installation through ModsBeforeFriday or
QuestPatcher on Beat Saber Quest `1.40.8_7379`.

### QPM Troubleshooting

If `qpm restore` fails before dependency download, validate the package file:

```sh
qpm package format
python3 -m json.tool qpm.json
python3 -m json.tool mod.template.json
```

If restore fails with DNS or tunnel errors for `qpackages.com`, check whether a
proxy is forcing QPM through an unreachable host:

```sh
getent hosts qpackages.com
printf '%s\n' "$HTTPS_PROXY"
curl -I https://qpackages.com/custom-types?limit=0
env -u HTTP_PROXY -u HTTPS_PROXY -u http_proxy -u https_proxy qpm restore
```

If direct access is blocked on your network, connect to the network/VPN that can
reach `qpackages.com`, or fix the `HTTP_PROXY`/`HTTPS_PROXY` values before
running `qpm restore`.

`config-utils` is intentionally not used because the available QPM package
versions conflict with the Quest `custom-types 0.18.3` runtime dependency.

If `qpm doctor` reports missing ADB or Android NDK:

```sh
qpm download adb
qpm ndk resolve --download
qpm doctor
```

For an NDK installed outside QPM, either set `ANDROID_NDK_HOME` or run:

```sh
qpm config ndk-path /path/to/android-ndk
```

## Manual Test Plan

- Build validation: `qpm restore`, `qpm s build`, and `qpm s qmod`.
- Install validation: install the `.qmod` on Beat Saber Quest `1.40.8_7379`.
- Runtime smoke: launch the game with BeatSinger enabled.
- Verify a custom song with `lyrics.json` displays timed lyrics.
- Verify a custom song with `lyrics.srt` displays timed lyrics.
- Verify missing lyrics logs a clear message and does not crash.
- Verify config changes persist after restart.
- Verify the HTTP provider makes no request while `httpEnabled` is false.
- Verify the HTTP provider handles JSON, SRT, 404, and timeout responses.
