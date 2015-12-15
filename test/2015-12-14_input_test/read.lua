stepstheta = 9
stepsphi = 9

s = {}

function docalculation (p) 
    local l = s[0] 
    local xoffset = s[1]
    local yoffset = s[2]
    local zoffset = s[3]
    local r = s[4]

    for i=0,stepstheta,1 do 
        for j=0,stepsphi,1 do 
            local theta = i * (1.0/stepstheta)* math.pi
            local phi   = j *  (1.0/stepsphi)* math.pi * 2. - math.pi
            local x = xoffset + math.sin(theta)*math.cos(phi)*r
            local y = yoffset + math.sin(theta)*math.sin(phi)*r
            local z = zoffset + math.cos(theta)*r
            p:addpoint(x, y, z, l)
        end
    end
end
