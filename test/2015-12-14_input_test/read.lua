stepstheta = 19
stepsphi = 19

s = {}

function docalculation (p) 
    local l = s[0] 
    local xoffset = s[1]
    local yoffset = s[2]
    local zoffset = s[3]
    local r = s[4]

    for i=0,stepstheta,1 do 
        for j=0,stepsphi,1 do 
            local theta = i * (1.0/stepstheta)* math.pi * 2.0
            local phi = math.acos(j *  (2.0/stepsphi) - 1.0)
            local x = xoffset + math.cos(theta)*math.sin(phi)*r
            local y = yoffset + math.sin(theta)*math.sin(phi)*r
            local z = zoffset + math.cos(phi)*r
            p:addpoint(x, y, z, l)
        end
    end
end
