import React, {Component} from 'react';
import {Link} from "react-router-dom";
//
import mainLogo from'../assets/tea.svg';
import user from'../assets/user.svg';
import shoppingCart from'../assets/shopping-cart.svg';
import logout from'../assets/logout.svg';
//
import {ROUTE_PREFIX, getUser} from '../const.js'

class Header extends Component {
    constructor(props) {
        super(props);
        // pro stav
        this.state = {
        	openingHours	: {
        		remaining	: '',
        		className	: 'opened'
        	},
        	dropdown: undefined,
            mode: this.props.mode,
            userName: ''
        }
        //
        this.refreshTimeInterval = 1000*60
        this.notifMessages = {
        	'closed' 		: 'Máme zavřeno',
        	'closesSoon'	: 'Pospěšte si, zavíráme',
        	'opensSoon' 	: 'Za chvilku otevíráme',
        	'opened' 		: 'Máme otevřeno'
        }
    }

    componentDidMount() {
    	this.handleTimeChange()
    }

    componentDidUpdate(prevProps) {
        if (this.props !== prevProps) {
            this.setState({
                mode: this.props.mode
            })
        }
    }

    handleTimeChange() {
    	this.storeAvailability()
    	// je treba periodiky obnovit
    	setInterval(() => {
    		this.storeAvailability()
    	}, this.refreshTimeInterval)
    }

    storeAvailability(currentTime) {
    	// var event = new Date()

    	// var day = event.getDay()
    	// var hour = event.getHours()
    	// var minutes = event.getMinutes()

    	//console.log(minutes)

		// if (day > 0 && day < 6) {
		// 	if (hour > 7 && hour < 17) {
		// 		console.log('ok')
		// 	} else {
		// 		console.log('nesedi hodiny')
		// 	}
		// } else {
		// 	console.log('nesedi dny')
		// }

		//console.log(event.toLocaleDateString('cs-CS', options))
    }

    handleChange = (id, e) => {
    	e.preventDefault()

    	let dropdown = this.state.dropdown
    	if (!dropdown) {
    		this.setState({'dropdown': id})	
    	} else {
    		if (dropdown === id) {
    			this.setState({'dropdown': undefined})
    		} else {
    			this.setState({'dropdown': id})	
    		}
    	}
    }

    render() {

    	var openingHours = this.state.openingHours

    	return (
			<header className="header">
				<div className="header__inner layout__inner">
					{/* Leva cast */}
					<div className="header__info">
						<ul>
							<li>10 - 19h <small className={`header__openHours ${openingHours.className}`}>[Zavřeno]</small></li>
							<li>+420 123 456 789</li>
							<li>BRNO, KRÁLOVO POLE, 258/1</li>
						</ul>
					</div>
					
					{/* Logo */}
					<Link to={`${ROUTE_PREFIX}`} className="header__logo">
						<img src={mainLogo} alt="" />
					</Link>
					
					{/* Prava cast*/}
					<HeaderNav mode={this.state.mode}></HeaderNav>
{/*					<div className={this.state.dropdown === HEADER_USER ? "header__dropdown active_user" :  this.state.dropdown === HEADER_STORE ? "header__dropdown active_store" : "header__dropdown inactive"}>
						<div className="arrow-up"></div>
						<p>{this.state.dropdown === HEADER_USER ? "Chtěl by ses přihlásit, co?" : "Máš prázdný košík Tatare!"}</p>
					</div>*/}
				</div>
			</header>
		);
    }
}


const HeaderNav = (mode) => {
    const appMode = mode.mode
    //console.log(`caje/${data.kategorie}/${data.druh_id}`)
    if (appMode === 'basic') {
         return null
    } else {
        var userInfo = getUser() ? getUser() : []
        var level = userInfo ? userInfo.level : undefined

        if (level > 3) {
            return (
                <nav className="header__control">
                    <Link to={`${ROUTE_PREFIX}admin`} className="header__control--double">
                        <img src={user} alt="Uživatel"/>
                        <div className="header__control__small">
                            <h6>{userInfo.name}</h6>
                            <small>přejít do administrace</small>
                        </div>
                    </Link>
                    <Link to={`${ROUTE_PREFIX}logout`} className="header__control--single">
                        <img src={logout} alt="Uživatel"/>
                        <small>odhlásit se</small>
                    </Link>
                </nav>
            ); 
        } else {
           return (
                <nav className="header__control">
                    <Link to={`${ROUTE_PREFIX}uzivatel`} className="header__control--double">
                        <img src={user} alt="Uživatel"/>
                        <div className="header__control__small">
                            <h6>{userInfo.name}</h6>
                            <small>{userInfo.level === 1 ? 'Maloobchodník' : 'Velkoobchodník'}</small>
                        </div>
                    </Link>
                    <Link to={`${ROUTE_PREFIX}kosik`}>
                        <img src={shoppingCart} alt="Košík"/>
                        <small>Váš košík</small>
                    </Link>
                </nav>
            ); 
        }
    }
}

export default Header;