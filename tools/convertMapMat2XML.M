% File:      convertMapMat2XML.m
% Author:    Lucas Koch (koch_luc@mmp.rwth-aachen.de)
%            Teaching and Research Area Mechatronics in Mobile Propulsion (MMP)
%            RWTH Aachen University
% Copyright: Eclipse Public License v2.0
%            (https://www.eclipse.org/legal/epl-2.0/)
% MATLAB script for conversion of powerloss maps generated with the electric
% machine design tool developed by Kalt et. al. (DOI: 10.1109/EVER.2019.8813601)
% into xml-format required for the MMPEVEM emission model in SUMO.
% If you use this model for academic research, you are highly encouraged to
% cite our paper "Accurate physics-based modeling of electric vehicle energy
% consumption in the SUMO traffic microsimulator"
% (DOI: 10.1109/ITSC48978.2021.9564463).

% After execution of the TUM Tool GUI,
% the results contains these three structs:
%       Maschinendaten
%       handles_Kennfeld
%       handles
% For conversion of the motor's power loss map, only the Maschinendaten struct
% needs to be available in the workspace.


% Specify path and a filename for xml
savePath = fullfile(pwd,"export.xml");

if exist("Maschinendaten")
    % struct Maschinendaten contains only data for Tq > 0 & n > 0 
    % Mirror losses to negative torque-axis
    tq_axis_pos = Maschinendaten.Kennfeld.M_max_mesh(:,1);
    pwrMotLoss_pos = Maschinendaten.Kennfeld.Verluste.P_vges_mesh;
    tq_axis = [-flip(tq_axis_pos); tq_axis_pos(2:end)];
    pwrMotLoss = [flip(pwrMotLoss_pos); pwrMotLoss_pos(2:end,:)];
    
    %convert n_axis to rpm 
    n_max = Maschinendaten.Optionen.n_max; % [rpm]
    n_axis = Maschinendaten.Kennfeld.omega_k_mesh(1,1:end);
    n_axis = n_max/max(n_axis)*n_axis;
   
    tmp = "2,1|";
    % write n_axis-vector 
    for i = 1:length(n_axis)
        tmp = append(tmp, num2str(n_axis(i)));
        if i ~= length(n_axis)
            tmp = append(tmp,",");
        else
            tmp = append(tmp,";");
        end
    end
    
    % write tq_axis-vector
    for i = 1:length(tq_axis)  
        tmp = append(tmp, num2str(tq_axis(i)));
        if i ~= length(tq_axis)
            tmp = append(tmp,",");
        else
            tmp = append(tmp,"|");
        end
    end
    
    % fill NaNs with last non-NaN value on Tq-axis
    for j = 1:length(n_axis)
        if isnan(pwrMotLoss(1,j))
            idx1 = find(~isnan(pwrMotLoss(:,j)),1,'first');
            pwrMotLoss(1:idx1-1,j) = pwrMotLoss(idx1,j);
        end
        if isnan(pwrMotLoss(end,j))
            idx2 = find(~isnan(pwrMotLoss(:,j)),1,'last');
            pwrMotLoss(idx2+1:end,j) = pwrMotLoss(idx2,j);
        end
    end
    
    
    %write powerloss-data
    for i = 1:length(tq_axis)
        for j = 1:length(n_axis)
            if ~isnan(pwrMotLoss(i,j))
                tmp = append(tmp,num2str(round(pwrMotLoss(i,j),2)));
                if ~(i == length(tq_axis) && j == length(n_axis))
                    tmp = append(tmp,",");
                end
            end
        end
    end

    %open docNode and append data
    docNode = com.mathworks.xml.XMLUtils.createDocument('routes');
    routes = docNode.getDocumentElement;
    vType = docNode.createElement('vType');
    routes.appendChild(vType);
    param = docNode.createElement('param');
    param.setAttribute('key','powerLossMap');
    param.setAttribute('value',tmp);
    vType.appendChild(param);
    xmlwrite(savePath,docNode);

    %open saved xml 
    winopen(savePath);

else
	disp('Maschinendaten missing. Please load them into the workspace.');
end

