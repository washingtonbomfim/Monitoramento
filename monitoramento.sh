#/bin/bash

if [ $1 = "stop" ]; then

	echo "finalizando Monitoramento..";
	killall monitoramento;

elif [ $1 = "status" ]; then

	status=$(ps aux | grep -i monitoramento | wc -l);
	if [$status -ge 2]; then
		echo "Monitoramento está em execução!";
	else
		echo "Monitoramento não está em execução!";
else
	./monitoramento $1;
fi
	

