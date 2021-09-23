#!/bin/bash
set -x
#set -e


#VARSETTINGS
configdirin=/usr/share/pipewire/media-session.d/alsa-monitor.conf           #pipewire system config
configdirout=~/.config/pipewire/media-session.d/alsa-monitor.conf           #pipewire user config
configdirtmp=~/.config/pipewire/media-session.d/alsa-monitor.tmp            #pipewire tmp write config
sampleratesettings=(Default 44100 48000 88200 96000 176400 192000)          #Samplerate avaiable for settings
buffersettings=(Default 32 64 128 256 512 1024 2048 4096 8192)              #Buffer avaiable for settings


#CORE
currentsrate=$(pw-metadata -n settings | awk '/[^F]clock.force-rate/ { print $4 }' FS="'" OFS="");
currentbuffer=$(pw-metadata -n settings | awk '/[^F]clock.force-quantum/ { print $4 }' FS="'" OFS="");

if [[ $currentsrate -ne 0 ]]; then
    samplerate=$(IFS=! ; echo "${sampleratesettings[*]}" | awk '$1=$1' FS=$currentsrate OFS="^"$currentsrate);
else
    samplerate=$(IFS=! ; echo "${sampleratesettings[*]}");
fi

if [[ $currentbuffer -ne 0 ]]; then
    buffer=$(IFS=! ; echo "${buffersettings[*]}" | awk '$1=$1' FS=$currentbuffer OFS="^"$currentbuffer);
else
    buffer=$(IFS=! ; echo "${buffersettings[*]}");
fi

if [[ -f $configdirout ]]; then
    alsabatchcurrent=$(awk '{if ($1 == "#api.alsa.disable-batch") print "false"; else if ($1 == "api.alsa.disable-batch") print $3;}' $configdirout);
    alsaperiodcurrent=$(awk '{if ($1 == "#api.alsa.period-size") print "1024"; else if ($1 == "api.alsa.period-size") print $3;}' $configdirout);
else
    alsabatchcurrent=$(awk '{if ($1 == "#api.alsa.disable-batch") print "false"; else if ($1 == "api.alsa.disable-batch") print $3;}' $configdirin);
    alsaperiodcurrent=$(awk '{if ($1 == "#api.alsa.period-size") print "1024"; else if ($1 == "api.alsa.period-size") print $3;}' $configdirin);
fi

#GUI
choice=$(yad --center --title "Pipewire DSP Settings" --window-icon=audio-card --image=pipewire-logo \
--borders=6 --width=400 \
--text="PIPEWIRE DSP SETTINGS" \
--separator="," \
--form \
--field="--- Force Session Settings (Default for reset) ---:LBL" \
--field="Sample Rate:CB" \
--field="Buffer:CB" \
--field="--- Advanced ALSA Systemwide options ---:LBL" \
--field="Disable 'Alsa Batch' (USB Device) extra buffer:CHK" \
--field="'Alsa Batch' extra buffer" \
--field="pipewire-media-session service restart only if you change alsa settings:LBL" \
text \
$(IFS=! ; echo "${samplerate[*]}") \
$(IFS=! ; echo "${buffer[*]}") \
text \
$alsabatchcurrent \
$alsaperiodcurrent \
--button="Okay:10" \
--button="Exit:30");

#APPLYSETTINGS
result=$?
sampleratechoice=$(echo $choice | awk 'BEGIN {FS="," } { print $2 }');
bufferchoice=$(echo $choice | awk 'BEGIN {FS="," } { print $3 }');
disablebatchchoice=$(echo $choice | awk 'BEGIN {FS="," } { print $5 }');
alsaperiodchoice=$(echo $choice | awk 'BEGIN {FS="," } { print $6 }');

if [[ $result -eq 10 ]]; then
    if [[ "$bufferchoice" == "Default" ]]; then
        echo
        echo Setting Buffer to default
        bufferchoice=0
        echo
    fi

    if [[ "$sampleratechoice" == "Default" ]]; then
        echo Setting SampleRate to Default
        sampleratechoice=0
        echo
    fi

    echo
    echo BUFFER: $bufferchoice
    echo SAMPLERATE: $sampleratechoice
    echo

    pw-metadata -n settings 0 clock.force-rate $sampleratechoice
    pw-metadata -n settings 0 clock.force-quantum $bufferchoice

    echo
    echo Current SampleRate: $currentsrate
    echo Current Buffer: $currentbuffer
    echo
    echo Current Batch Disabled State: $alsabatchcurrent
    echo Current Alsa Period: $alsaperiodcurrent
    echo

    if [ ! -f $configdirout ]; then
    mkdir -p ~/.config/pipewire/media-session.d && cp $configdirin $configdirout;
    fi

    if [[ -f $configdirout ]]; then
        echo "Config file in position!"

        awk -v awkalsaperiodset="$alsaperiodchoice" '{if ($1 == "api.alsa.period-size") gsub($3, awkalsaperiodset); else if ($1 == "#api.alsa.period-size") gsub($1, "api.alsa.period-size") gsub($3, awkalsaperiodset)}1' $configdirout > $configdirtmp && mv $configdirtmp $configdirout
        awk -v awkdisablebatch="$disablebatchchoice" '{if ($1 == "api.alsa.disable-batch") gsub($3, awkdisablebatch); else if ($1 == "#api.alsa.disable-batch") gsub($1, "api.alsa.disable-batch") gsub($3, awkdisablebatch)}1' $configdirout > $configdirtmp && mv $configdirtmp $configdirout
    else
        awk -v awkalsaperiodset="$alsaperiodchoice" '{if ($1 == "api.alsa.period-size") gsub($3, awkalsaperiodset); else if ($1 == "#api.alsa.period-size") gsub($1, "api.alsa.period-size") gsub($3, awkalsaperiodset)}1' $configdirin > $configdirout
        awk -v awkdisablebatch="$disablebatchchoice" '{if ($1 == "api.alsa.disable-batch") gsub($3, awkdisablebatch); else if ($1 == "#api.alsa.disable-batch") gsub($1, "api.alsa.disable-batch") gsub($3, awkdisablebatch)}1' $configdirin > $configdirout
    fi

    if [[ $alsaperiodcurrent -ne $alsaperiodchoice ]] || [[ "$alsabatchcurrent" != "$disablebatchchoice" ]]; then
    #if [[ $alsaperiodcurrent -ne $alsaperiodchoice ]]; then
    echo
    echo Restart pipewire-media-session
    echo
    systemctl --user restart pipewire-media-session
    fi

fi

if [[ $result -eq 20 ]]; then
    echo
    echo Reset Settings
    echo
fi

if [[ $result -eq 30 ]]; then
    echo exit
fi
