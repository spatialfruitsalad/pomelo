stepst = 10
stepsphi = 10
shrink = 0.6

s = {}

function docalculation (p) 
    local l = s[0] 
    local xoffset = s[1]
    local yoffset = s[2]
    local zoffset = s[3]
    local ux = s[4]
    local uy = s[5]
    local uz = s[6]

    local a11 = 1
    local a22 = 1
    local a33 = 1
    local a12 = 0
    local a13 = 0
    local a21 = 0
    local a23 = 0
    local a31 = 0
    local a32 = 0
    if uz*uz ~= 1 then
        local sqrtz = 1.0/math.sqrt(1.0-uz*uz);
        a11 = ux*uz*sqrtz
        a21 = uy*uz*sqrtz
        a31 = -1.0/sqrtz
        a12 = -uy*sqrtz
        a22 = ux*sqrtz
        a32 = 0
        a13 = ux
        a23 = uy
        a33 = uz
    elseif uz == -1 then
        a11 = 1
        a22 = 1
        a33 = -1
        a12 = 0
        a21 = 0
        a31 = 0
        a13 = 0
        a23 = 0
        a32 = 0
    end

    for i=0,stepst,1 do 
        for j=0,stepsphi,1 do
            local x = 0
            local y = 0
            local z = 0

            local nx = 0
            local ny = 0
            local nz = 0

            if i==0 then
                x = 0
                y = 0
                z= -1.5/math.sqrt(2)

                nx = 0
                ny = 0
                nz = -1
            elseif i == stepst then
                x = 0 
                y = 0 
                z = 1.5/math.sqrt(2)
                      
                nx = 0
                ny = 0
                nz = 1

            elseif i > 0.5*stepst then
                local t=i*1.0/stepst-0.5
                local tt=1.0-t
                local phi = j*2.0*math.pi/stepsphi

                local r = (tt*tt*tt*0.5 + 6.0/19.0*t*tt*tt - 6.0/19.0*t*t*tt - t*t*t*0.5)/math.sqrt(2)
                
                x = r*math.cos(phi)
                y = r*math.sin(phi)
                z = 6.0*t*tt/math.sqrt(2)
            
                local nzPear = 45.0/38.0*tt*tt + 24.0/19.0*t*tt + 45.0/38.0*t*t
                local nrhoPear = 6.0*tt-6.0*t

                local nr = nzPear*nzPear + nrhoPear*nrhoPear
                nr = math.sqrt(nr)
                nzPear = nzPear/nr
                nrhoPear = nrhoPear/nr
                
                nx = nrhoPear*math.cos(phi)
                ny = nrhoPear*math.sin(phi)
                nz = nzPear

            else
               local t= 0.5-i*1.0/stepst
                local tt=1.0-t
                local phi = j*2.0*math.pi/stepsphi

                local r = (tt*tt*tt*0.5 + 51.0/19.0*t*tt*tt - 51.0/19.0*t*t*tt - t*t*t*0.5)/math.sqrt(2)
                
                x = r*math.cos(phi)
                y = r*math.sin(phi)
                z = -6*t*tt/math.sqrt(2)
                
                local nzPear = 45.0/38.0*tt*tt - 204.0/19.0*t*tt + 45.0/38.0*t*t
                local nrhoPear = 6.0*tt-6.0*t

                local nr = nzPear*nzPear + nrhoPear*nrhoPear
                nr = math.sqrt(nr)
                nzPear = nzPear/nr
                nrhoPear = nrhoPear/nr
                
                nx = nrhoPear*math.cos(phi)
                ny = nrhoPear*math.sin(phi)
                nz = nzPear
            end
           
            x = x*shrink
            y = y*shrink 
            z = z*shrink


            local xn = xoffset + a11*x + a12*y + a13*z
            local yn = yoffset + a21*x + a22*y + a23*z
            local zn = zoffset + a31*x + a32*y + a33*z
            p:addpoint(xn, yn, zn, l)
        end
    end
end
