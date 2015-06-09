function Hash = md5(Data)
if ~usejava('jvm')
   error(['JSimon:', mfilename, ':NoJava'], ...
      '*** %s: Java is required.', mfilename);
end
persistent usetypecastx
if isempty(usetypecastx)
   usetypecastx = ~isempty(which('typecastx'));
end
try
   Engine = java.security.MessageDigest.getInstance('MD5');
catch
   error(['JSimon:', mfilename, ':BadInput2'], ...
      '*** %s: Invalid algorithm: [%s].', mfilename, 'MD5');
end
if usetypecastx
   Engine = CoreHash_(Data, Engine);
   Hash   = typecastx(Engine.digest, 'uint8');
else
   Engine = CoreHash(Data, Engine);
   Hash   = typecast(Engine.digest, 'uint8');
end
Hash = sprintf('%.2X', double(Hash));

function Engine = CoreHash_(Data, Engine)
Engine.update([uint8(class(Data)), typecastx(size(Data), 'uint8')]);
if isstruct(Data)
   F      = sort(fieldnames(Data));
   Engine = CoreHash_(F, Engine);
   for iS = 1:numel(Data)
      for iField = 1:length(F)
         Engine = CoreHash_(Data(iS).(F{iField}), Engine);
      end
   end
elseif iscell(Data)
   for iS = 1:numel(Data)
      Engine = CoreHash_(Data{iS}, Engine);
   end
      
elseif isnumeric(Data) || islogical(Data) || ischar(Data)
   if isempty(Data) == 0
      if isreal(Data)
         Engine.update(typecastx(Data(:), 'uint8'));
      else
         Engine.update(typecastx(real(Data(:)), 'uint8'));
         Engine.update(typecastx(imag(Data(:)), 'uint8'));
      end
   end
elseif isa(Data, 'function_handle')
   Engine = CoreHash(ConvertFuncHandle(Data), Engine);
else
   try
      Engine = CoreHash(ConvertObject(Data), Engine);
   catch
      warning(['JSimon:', mfilename, ':BadDataType'], ...
         ['Type of variable not considered: ', class(Data)]);
   end
end

function Engine = CoreHash(Data, Engine)
Engine.update([uint8(class(Data)), typecast(size(Data), 'uint8')]);
if isstruct(Data)
   F      = sort(fieldnames(Data));
   Engine = CoreHash(F, Engine);
   for iS = 1:numel(Data)
      for iField = 1:length(F)
         Engine = CoreHash(Data(iS).(F{iField}), Engine);
      end
   end
elseif iscell(Data)
   for iS = 1:numel(Data)
      Engine = CoreHash(Data{iS}, Engine);
   end
elseif isempty(Data)
elseif isnumeric(Data)
   if isreal(Data)
      Engine.update(typecast(Data(:), 'uint8'));
   else
      Engine.update(typecast(real(Data(:)), 'uint8'));
      Engine.update(typecast(imag(Data(:)), 'uint8'));
   end
elseif islogical(Data)
   Engine.update(typecast(uint8(Data(:)), 'uint8'));
elseif ischar(Data)
   Engine.update(typecast(uint16(Data(:)), 'uint8'));
elseif isa(Data, 'function_handle')
   Engine = CoreHash(ConvertFuncHandle(Data), Engine);
else 
   try
      Engine = CoreHash(ConvertObject(Data), Engine);
   catch
      warning(['JSimon:', mfilename, ':BadDataType'], ...
         ['Type of variable not considered: ', class(Data)]);
   end
end

function FuncKey = ConvertFuncHandle(FuncH)
FuncKey = functions(FuncH);

function DataBin = ConvertObject(DataObj)
DataBin = uint8(DataObj);
