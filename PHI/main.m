Config.Debug = 0;
Config.TaskPath = 'Part.task';
Config.Height = 100;
Config.Width = 80;
Config.PatchLength = 20;
Config.ShiftLength = 10;
Config.BitNum = 8;
Config.BinNum = 9;
Config.GridNum = 4;
Config.EvaluateNum = 1000;
Config.Lambda = 0.1;

load('3_32.mat', 'SHparam');
Config.SHparam = SHparam;
Config.GridNum = 3;
Config.Height = 100;
Config.Width = 80;
Feature = generate_feature(Config, 'm_parts/anklet_0_0.jpg');
Bits = compressSH(Feature, SHparam);

% % Precision = zeros([0, Config.EvaluateNum]);
% % for BitNum = 8 : 8 : 32
% %     for GridNum = 2 : 3
% %         Config.BitNum = BitNum;
% %         Config.GridNum = GridNum;
% %         Config = train_phi(Config);
% %         
% %         SHparam = Config.SHparam;
% %         save([num2str(GridNum) '_' num2str(BitNum) '.mat'], 'SHparam');
% % 
% %         if Config.Debug == 1
% %             visual_hash(Config);
% %         end
% % 
% %         %Path = test_phi(Config);
% %         %Precision(end + 1, :) = evaluate_phi(Config, Path);
% %         %plot(1 : Config.EvaluateNum, Precision);
% %         %drawnow;
% %     end
% % end

% for BitNum = 8 : 8 : 32
%     for GridNum = 2 : 3
%         load([num2str(GridNum) '_' num2str(BitNum) '.mat'], 'SHparam');
%         File = fopen([num2str(GridNum) '_' num2str(BitNum) '.param'], 'w');
%         DimNum = GridNum * GridNum * Config.BinNum; 
%         fprintf(File, '%d %d\n', DimNum, BitNum);
%         for i = 1 : DimNum
%             for j = 1 : BitNum
%                 fprintf(File, '%.6f ', SHparam.pc(i, j));
%             end
%             fprintf(File, '\n');
%         end
%         for i = 1 : BitNum
%             fprintf(File, '%.6f ', SHparam.mn(i));
%         end
%         fprintf(File, '\n');
%         for i = 1 : BitNum
%             fprintf(File, '%.6f ', SHparam.mx(i));
%         end
%         fprintf(File, '\n');
%         for i = 1 : BitNum
%             for j = 1 : BitNum
%                 fprintf(File, '%d ', SHparam.modes(i, j));
%             end
%             fprintf(File, '\n');
%         end
%         fclose(File);
%     end
% end
% 
% Config.Debug = 0;
% Config.TaskPath = 'PI100.task';
% Config.Height = 200;
% Config.Width = 200;
% Config.PatchLength = 20;
% Config.ShiftLength = 10;
% Config.BitNum = 8;
% Config.BinNum = 9;
% Config.GridNum = 4;
% Config.EvaluateNum = 1000;
% Config.Lambda = 0.1;
% 
% Precision = zeros([0, Config.EvaluateNum]);
% for BitNum = 8 : 8 : 32
%     for GridNum = 2 : 4
%         Config.BitNum = BitNum;
%         Config.GridNum = GridNum;
%         Config = train_phi(Config);
%         Path = test_phi(Config);
%         Precision(end + 1, :) = evaluate_phi(Config, Path);
%         plot(1 : Config.EvaluateNum, Precision);
%         drawnow;
%     end
% end