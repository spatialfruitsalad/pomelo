stepstheta = 4 
stepsphi = 4
shrink = 0.1

s = {}

function docalculation (p) 
    local l = s[0] 
    local xoffset = s[1]
    local yoffset = s[2]
    local zoffset = s[3]
    local r = s[4]*shrink

    for i=0,stepstheta,1 do 
        for j=0,stepsphi,1 do 
            local theta = i * (1.0/stepstheta)* math.pi * 2.0
            local phi = math.acos(j *  (2.0/stepsphi) - 1.0)
            local x = xoffset + math.cos(theta)*math.sin(phi)*(r*0.97)
            local y = yoffset + math.sin(theta)*math.sin(phi)*(r*1.05)
            local z = zoffset + math.cos(phi)*(r * 0.95)
            p:addpoint(x, y, z, l)
        end
    end
end
