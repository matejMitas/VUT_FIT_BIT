import React from 'react';
import {Link} from "react-router-dom";
import {ROUTE_PREFIX} from '../const.js'

import logo404 from '../assets/no-water.svg';

const errorPage = () => {
    return (
        <main className="error">
        	<div className="layout__inner">
        		<div className="error__wrapper">
	        		<img src={logo404} alt="" />
	           		<h1>Bez vody není čaje. A voda došla.</h1>
	           		<p>Asi se chcete vrátit na hlavní stranu, že?</p>
	            	<Link to={`${ROUTE_PREFIX}`}>Zpět</Link>
        		</div>
        	</div>
        </main>
    )
}

export default errorPage;