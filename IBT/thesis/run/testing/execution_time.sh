BASEDIR=$(pwd)
BASEDIR=${BASEDIR%/*/*}
BASEBASEDIR=${BASEDIR%/*}

asn_lib=$BASEBASEDIR/ipfixcol2/build/src/plugins/intermediate/asn/libasn-intermediate.so
opt_lib=$BASEBASEDIR/ipfixcol2/build/src/plugins/intermediate/optimized/liboptimize-intermediate.so

asn_normal=$BASEDIR/run/startups/ipfix_asn_dummy.xml
geo_normal=$BASEDIR/run/startups/ipfix_geoip_dummy.xml
ipfix_normal=$BASEDIR/run/startups/original/ipfix_dummy.xml

asn_templates=$BASEDIR/run/startups/templates/ipfix_asn_dummy.xml
geo_templates=$BASEDIR/run/startups/templates/ipfix_geoip_dummy.xml
ipfix_templates=$BASEDIR/run/startups/templates/ipfix_dummy.xml

command="time /home/adam/ipfixcol2/build/src/core/ipfixcol2 -c $asn_templates -p $opt_lib"

echo "$command"
for i in {1..5}
do
    output=$({ eval $command >/dev/null; } |& grep user)
    curr_time=$(echo $output | awk '{print(substr($2, 3, 5)); print(" ")}')
    user_time+=$curr_time
    echo "run $i: "$curr_time
done

echo $user_time | awk '{sum = 0; for (i = 1; i <= NF; i++) sum += $i; sum /= NF; print("Overall time:", sum)}'